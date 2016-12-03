#include "servo.h"

void servo_timer_init(){
	SYSCTL_RCGCGPIO_R |= BIT1; // Turn on clock to GPIOportB5
	GPIO_PORTB_DIR_R |= BIT5; //PB5 to output

	GPIO_PORTB_AFSEL_R |= BIT5;
	GPIO_PORTB_PCTL_R = 0x700000; //T1CCP1

	GPIO_PORTB_DEN_R |= 0b00100000;

	SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1; //turn on clk for timer1

	TIMER1_CTL_R &= 0xFEFF; //disable timer to config

	TIMER1_CFG_R = TIMER_CFG_16_BIT; //set size of timer to 16
	TIMER1_TBMR_R |= 0b00001010; //periodic and PWM enable

	int pulse_period = 320000;

	TIMER1_TBILR_R = pulse_period & 0xFFFF; //lower 16 bits of the interval
	TIMER1_TBPR_R = pulse_period >> 16; //set the upper 8 bits of the interval

	TIMER1_CTL_R |= 0x0100; //enable timer

	move_servo(90);
	timer_waitMillis(500);
}

//void move_servo(int degree){
//	unsigned int out = degree*(160) + 6400;
//
//	TIMER1_TBMATCHR_R = (320000 - out) & 0xFFFF;
//	TIMER1_TBPMR_R = (320000 - out) >> 16;
//}

void move_servo(int degree){
	unsigned pulse_period = 16000000*0.02;
	unsigned pulse_width; //pulse width in cycles
	//calculate pulse width in cycles
	pulse_width = 16000000*(degree*0.00185/180 + 0.00045);//programmed for cybot 9
	TIMER1_TBMATCHR_R = (pulse_period - pulse_width) & 0xFFFF;
	TIMER1_TBPMR_R = (pulse_period - pulse_width) >> 16;
}



