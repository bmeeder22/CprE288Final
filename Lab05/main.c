#include "lcd.h"
#include "timer.h"
#include "button.h"

char uart_receive(void);
void uart_init(void);
void mainloop(void);
void uart_sendChar(char data);
int buttonCheck(int past);
void uart_sendString(char data[]);
void WiFi_start();

int main(void) {
	lcd_init();
	button_init();
	uart_init();
	WiFi_start();
	
	while(1) {
		mainloop();
	}
}

void mainloop(void) {
	int i;
	char inputs[21];
	for(i=0; i<20; i++) {
		char received = uart_receive();
		if(received == '\r') {
			i--;
			uart_sendChar(10);
			uart_sendChar('\r');
			continue;
		}

		lcd_clear();
		lcd_printf("%d: %c", i+1, received);

		inputs[i] = received;
		uart_sendChar(received);
	}
	inputs[20] = '\0';

	lcd_clear();
	lcd_puts(inputs);
}
int buttonCheck(int past){
	int pressed = button_getButton();
	if(pressed == past) {
		return past;
	}

	if (pressed == 1 && 1 != past) {
		uart_sendString("Yes\n\r");
		return 1;
	}
	else if (pressed == 2 && 2 != past) {
		uart_sendString("No\n\r");
		return 2;
	}
	else if (pressed == 3 && 3 != past) {
		uart_sendString( "Blue, no green, Ahhhhh!!!\n\r");
		return 3;
	}
	else if (pressed == 4 && 4 != past) {
		uart_sendString("The force is strong with this one\n\r");
		return 4;
	}
	else if (pressed == 5 && 5 != past) {
		uart_sendString("Do not go gentle into that good night\n\r");
		return 5;
	}
	else if (pressed == 6 && 6 != past) {
		uart_sendString("What's in the box\n\r");
		return 6;
	}
	return 0;
}

void uart_sendString(char data[]){
	int i=0;
	while(data[i] != '\0') {
		uart_sendChar(data[i]);
		i++;
	}
}

void uart_sendChar(char data){
	 //check to see if room to send
	 while((UART1_FR_R & 0x20) != 0);
	 //send
	 UART1_DR_R = data;
}

//blocking call to receive over uart1/
//returns char with data
char uart_receive(void){
	char data = 0;
	 //wait to receive
	 int past = 0;
	 while(UART1_FR_R & UART_FR_RXFE){
		 past = buttonCheck(past);
	 }
	 //mask the 4 error bits and grab only 8 data bits
	 data = (char)(UART1_DR_R & 0xFF);

	 return data;
}

void uart_init(void) {
	 //enable clock to GPIO, R1 = port B
	 SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	 //enable clock to UART1, R1 = UART1. ***Must be done before setting Rx and Tx
	 SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1;
	 //enable alternate functions on port b pins 0 and 1
	 GPIO_PORTB_AFSEL_R |= (BIT0 | BIT1);
	 //enable Rx and Tx on port B on pins 0 and 1
	 GPIO_PORTB_PCTL_R |= 0x00000011;
	 //set pin 0 and 1 to digital
	 GPIO_PORTB_DEN_R |= (BIT0 | BIT1);
	 //set pin 0 to Rx or input
	 GPIO_PORTB_DIR_R &= ~BIT0;
	 //set pin 1 to Tx or output
	 GPIO_PORTB_DIR_R |= BIT1;


	 //calculate baudrate
	 uint16_t iBRD = 8;
	 uint16_t fBRD = 44;


	 //turn off uart1 while we set it up
	 UART1_CTL_R &= ~(UART_CTL_UARTEN);
	 //set baud rate
	 UART1_IBRD_R = iBRD;
	 UART1_FBRD_R = fBRD;
	 //set frame, 8 data bits, 1 stop bit, no parity, no FIFO
	 UART1_LCRH_R = UART_LCRH_WLEN_8 ;
	 //use system clock as source
	 UART1_CC_R = UART_CC_CS_SYSCLK;
	 //re-enable enable RX, TX, and uart1
	 UART1_CTL_R = (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);
}

void WiFi_start() {
	GPIO_PORTB_DIR_R &= 0x00;
	GPIO_PORTB_DIR_R |= BIT2;
	GPIO_PORTB_DATA_R &= 0x00;
	GPIO_PORTB_DATA_R |= BIT2;

	uart_sendChar(0x00); //Send command
	uart_sendString("password"); //Send WiFi PSK
	uart_sendChar('\0'); //NULL terminator
	char response = uart_receive(); //Wait for response
	GPIO_PORTB_DIR_R &= BIT1;
}


