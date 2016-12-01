#include "sonar.h"

int count=0;
volatile unsigned rising_time; // start time of the return pulse
volatile unsigned falling_time; // end time of the return pulse

void sonar_timer_init(){
  	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; // Turn on clock to GPIOportB
  	GPIO_PORTB_DEN_R |= 0x08;

  	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3; // Turn on clock to TIMER3

  	//Configure the timer for input capture mode
  	TIMER3_CTL_R &= ~(TIMER_CTL_TBEN); //disable timerB to allow us to change settings
  	TIMER3_CFG_R |= TIMER_CFG_16_BIT; //set to 16 bit timer

  	TIMER3_CTL_R |= TIMER_CTL_TBEVENT_BOTH;
  	TIMER3_TBMR_R = 0x03 | 0x04 | TIMER_TBMR_TBCDIR; //set for capture mode & edge time mode pp735

  	//clear TIMER3B interrupt flags
  	TIMER3_ICR_R = (TIMER_ICR_TBTOCINT); //clears TIMER4 time-out interrupt flags
  	TIMER3_IMR_R |= (TIMER_IMR_CBEIM); //enable TIMER4(A&B) time-out interrupts

  	//initialize local interrupts
  	NVIC_EN1_R |= 0x10;

  	IntRegister(INT_TIMER3B, TIMER3B_Handler); //register TIMER4B interrupt handler
  	IntMasterEnable(); //intialize global interrupts
  }

void TIMER3B_Handler(void) {
 	int event_time = TIMER3_TBR_R;

 	if(count == 0) {
 		count=1;
 		rising_time = event_time;
 	}
 	else if(count == 1) {
 		count=2;
 		falling_time = event_time;
 	}

 	TIMER3_ICR_R |= TIMER_ICR_CBECINT;
 }

unsigned sonar_read() {
 	send_pulse();
 	while(count != 2);
 	count = 0;
 	unsigned distance = ping_read();
 	return distance;
}

unsigned time2dist(unsigned time) {
	double interval = time*17;
	int out = interval/1600;
	return out;
}

  unsigned ping_read() {
  	unsigned elapsed_time = falling_time-rising_time;
  	int distance = time2dist(elapsed_time);
  	return distance;
  }

  void send_pulse() {
  	//disable alternate functin of pin afsel & pctl
  	GPIO_PORTB_AFSEL_R &= 0xF7;
  	GPIO_PORTB_PCTL_R |= 0x7000;

  	GPIO_PORTB_DIR_R |= 0x08; // set PB3 as output

  	GPIO_PORTB_DATA_R |= 0x08; // set PB3 to high

  	// wait at least 5 microseconds based on data sheet
  	timer_waitMicros(5);

  	GPIO_PORTB_DATA_R &= 0xF7; // set PB3 to low

  	GPIO_PORTB_DIR_R &= 0xF7; // set PB3 as input

  	//enalble alternate function
  	GPIO_PORTB_AFSEL_R |= 0x08;
  	GPIO_PORTB_PCTL_R |= 0x7000;

  	TIMER3_CTL_R |= TIMER_CTL_TBEN; //Enable TIMER3B
  }
