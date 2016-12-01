#include "uart.h"

//globals used for testing
//counts how many bytes received
volatile uint32_t isr_counter = 0;
//buffer that loads bytes when isr receives them
volatile char isr_char_buffer[50];
//
int error_check;

//initializes uart1 on gpio port B pins 0 Rx and 1 Tx with a baud rate of 11520
void uart_init(void){
    //gpio B pin 0 RX pin 1 TX
    //UART 1
    //BAUD 115200

    //calculate baudrate
    uint16_t iBRD = 0x08;//BRD=SYSCLK/((ClkDiv)(BaudRate)), HSE=0 ClkDiv=16, BaudRate=115,200
	uint16_t fBRD = 44;//Fractional remainder is 0.6805, DIVFRAC = (.6805)(64)+0.5 = 44

    //enable clock to GPIO, R1 = port B
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    //enable clock to UART1, R1 = UART1
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

    //turn off uart1 while we set it up
    UART1_CTL_R &= ~(UART_CTL_UARTEN);

    //set baud rate
    UART1_IBRD_R = iBRD;
    UART1_FBRD_R = fBRD;

    //set frame organization, 8 data bits, 1 stop bit, no parity, no FIFO
    UART1_LCRH_R = UART_LCRH_WLEN_8 ;

    //use system clock as source
    UART1_CC_R = UART_CC_CS_SYSCLK;

    //clear interrupt flags
    UART1_ICR_R = (UART_ICR_TXIC | UART_ICR_RXIC);

    //enable send and receive raw interrupts
    //UART1_IM_R |= UART_IM_TXIM | UART_IM_RXIM;

    //set priority of usart1 to 1. group 1 bits 21-23
    NVIC_PRI1_R |= 0x00200000;

    //enable interrupt for IRQ 6 set bit 6
    NVIC_EN0_R |= 0x00000040;

    //use system clock as source
    UART1_CC_R = UART_CC_CS_SYSCLK;

    //tell cpu to use ISR handler for uart1
    IntRegister(INT_UART1, UART1_Handler);

    //enable global interrupts
    IntMasterEnable();

    //re-enable enable RX, TX, and uart1
    UART1_CTL_R = (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);
}

char* uart_receivePacket() {
	int i;
	char out[5] = "aaaa\0";
	for(i=0;i<4;i++) {
		out[i] = uart_receive();
	}
	return out;
}

//blocking call that sends 1 char over uart1
void uart_sendChar(char data){
    //check to see if empty
    while((UART1_FR_R & 0x20) != 0);

    //send
    UART1_DR_R = data;

}

void uart_sendBuffer(char* buffer, int length) {
	while((length--) > 0) {
		uart_sendChar(*(buffer++));
	}
}

//blocking call to receive over uart1/
//returns char with data
int uart_receive(void){
    int error_check;
    char data = 0;

    //wait to receive
   while((UART1_FR_R & UART_FR_RXFE));

    //grab the data and error flag bits
    error_check = UART1_DR_R;

    //check for error
    if (error_check & 0xF00){
    	error_check &= 0xF00;
    	return -1;
        //error occured
    }else{
    	data = (char)(UART1_DR_R & 0xFF);
    }

    return data;
}

void uart_flush(void) {
	volatile char dummy;

	while(!(UART1_FR_R & UART_FR_RXFE)) {
		//Flush FIFO
		dummy = (char)(UART1_DR_R & 0xFF);
	}
}

//sends entire char array over uart1
//input first element in char array
void uart_sendStr(const char *data){
    while(*data != '\0') {
        uart_sendChar(*data);
        data++;
    }

    uart_sendChar('\n');
}

void uart_sendStrNoNewline(const char *data) {
	while(*data != '\0') {
		uart_sendChar(*data);
		data++;
	}
}

void uart_sendNum(int num) {
	char str[15];
	sprintf(str, "%d", num);
	uart_sendStrNoNewline(str);
}

//Interrupt handler for uart1
void UART1_Handler(void){

	//received a byte
	if(UART1_MIS_R & UART_MIS_RXMIS){
		isr_char_buffer[isr_counter]= (char)(UART1_DR_R & 0xFF);
		isr_counter++;
		UART1_ICR_R |= UART_ICR_RXIC;
	}
	//sent a byte
	else if(UART1_MIS_R & UART_MIS_TXMIS){
		UART1_ICR_R |= UART_ICR_TXIC;
	}

}

