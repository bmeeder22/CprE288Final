/*
 * main.c
 */
#include "lcd.h"
#include "timer.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"

volatile enum {LOW, HIGH, DONE} state;
volatile unsigned rising_time; // start time of the return pulse
volatile unsigned falling_time; // end time of the return pulse

void send_pulse();
void TIMER3B_Handler(void);
void clock_timer_init(void);
unsigned ping_read();
unsigned time2dist(unsigned time);

int main(void) {
	lcd_init();
	clock_timer_init();
	IntRegister(INT_TIMER3B, TIMER3B_Handler); //register TIMER4B interrupt handler
	IntMasterEnable(); //intialize global interrupts

	while(1){
		send_pulse();
		timer_waitMillis(750);
		unsigned distance = ping_read();
		lcd_printf("%d", distance);
	}
}

/* ping sensor related to ISR */
void TIMER3B_Handler(void) {
	//check if rising or falling edge
	//if rising store time
	//if falling compute length of time and compute distance *** flag register **** MIS_R ****
	int event_time = TIMER3_TBR_R;
	if(TIMER3_CTL_R & 0x0C00 == 0x0000) {
		//rising
		rising_time = event_time;
	}
	else if(TIMER3_CTL_R & 0x0C00 == 0x0400) {
		//falling
		falling_time = event_time;
	}
}

unsigned ping_read() {
	unsigned elapsed_time = falling_time-rising_time;
	return time2dist(elapsed_time);
}

void clock_timer_init(void) {
	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3; // Turn on clock to TIMER3
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // Turn on clock to GPIOportB

	GPIO_PORTB_PCTL_R |= 0x00007000;

	//Configure the timer for input capture mode
	TIMER3_CTL_R &= ~(TIMER_CTL_TBEN); //disable timerB to allow us to change settings
	TIMER3_CFG_R |= TIMER_CFG_16_BIT; //set to 16 bit timer

	TIMER3_TBMR_R = 0x03 | 0x04; //set for capture mode & edge time mode pp735

	//tell it to look at edges
//	TIMER3_CTL_R = 0x0;
//	TIMER3_CFG_R =  0x0000.0004;
//	TIMER3_CTL_R = 0x03

	//clear TIMER3B interrupt flags
	TIMER3_ICR_R = (TIMER_ICR_TBTOCINT); //clears TIMER4 time-out interrupt flags
	TIMER3_IMR_R |= (TIMER_IMR_CBEIM); //enable TIMER4(A&B) time-out interrupts

	//initialize local interrupts
	NVIC_EN3_R |= 0x20; //#warning "enable interrupts for TIMER4A and TIMER4B" n = 0, 1, 2, 3, or 4   *********** bit5 of enable 3
	//NVIC_EN4_R = 0x000000C0;
	//go to page 105 and find the corresponding interrupt numbers for TIMER4 A&B
	//then set those bits in the correct interrupt set EN register (p. 142)

	TIMER3_CTL_R |= TIMER_CTL_TBEN; //Enable TIMER3B
}



void send_pulse() {
	SYSCTL_RCGCGPIO_R |= 0x02;

	//disable alternate functin of pin afsel & pctl
	GPIO_PORTB_AFSEL_R &= 0x0000;
	//GPIO_PORTB_PCTL_R  &= 0x0000;

	GPIO_PORTB_DIR_R |= 0x08; // set PB3 as output

	GPIO_PORTB_DEN_R |= 0x08;

	GPIO_PORTB_DATA_R |= 0x08; // set PB3 to high

	// wait at least 5 microseconds based on data sheet
	timer_waitMicros(5);

	GPIO_PORTB_DATA_R &= 0xF7; // set PB3 to low

	GPIO_PORTB_DIR_R &= 0xF7; // set PB3 as input

	//enalble alternate function
	GPIO_PORTB_AFSEL_R |= 0x08;
	//GPIO_PORTB_PCTL_R |= 0x00007000;

	while (!(TIMER3_MIS_R & TIMER_MIS_CBMIS));
	TIMER3B_Handler();
	//Write a 1 to TIMER3_ICR_CBECINT
	TIMER_ICR_CBECINT = 0b10000000000;

	//TIMER3_TBMR_R
}

unsigned time2dist(unsigned time) {
	return time/94;

}

