#include "lcd.h"
#include "timer.h"
#include "uart.h"
#include "wifi.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"
#include <math.h>
#include <string.h>

#define	M_PI		3.14159265358979323846

void sonar_timer_init();
void ir_timer_init();
void servo_timer_init();
void move_servo();
int ir_read();
unsigned sonar_read();
unsigned time2dist(unsigned time);
void TIMER3B_Handler(void);
unsigned ping_read();
void init_all();
double toRadians(double angle);
int radialToRealC(double radial_width, int distanceToObject);
void handleObjectFound();
void uart_sendNum(int num);

int degree;
int count = 0;
int degrees = 180;
int objCount = 0;

int smallestLocation = 10000;
int smallestWidth = 10000;

volatile unsigned rising_time; // start time of the return pulse
volatile unsigned falling_time; // end time of the return pulse

int main() {
	init_all();

	uart_sendStr("****************************************************\n\r");
	uart_sendStr("Degrees\tIRDistance(cm)\t Sonar Distance(cm)\n\r");

    while(degrees>20) {
        move_servo(degrees);
        degrees -= 2;

        int IRDist = ir_read();
        int SonarDist = sonar_read();
        SonarDist = SonarDist/10;

        uart_sendNum(degrees);
        uart_sendStr("\t");
        uart_sendNum(IRDist);
        uart_sendStr("\t");
        uart_sendStr("\t");
        uart_sendNum(SonarDist);
        uart_sendStr("\n\r");

        if(IRDist > 500) { // was if(SonarDist < 20 && SonarDist!=0)
            handleObjectFound();
        }

        timer_waitMillis(500);
    }

    int small = smallestLocation;
    move_servo(smallestLocation);
    timer_waitMillis(500);
}

void init_all() {
	lcd_init();
    sonar_timer_init();
	ir_timer_init();
	servo_timer_init();
	uart_init();

//	WiFi_start("aaaaaaaa\0");
//	while(1){}
}

void handleObjectFound() {
    unsigned int start = degrees;
    int IRDist = ir_read();
    int distance = sonar_read()/10;

    while(IRDist > 600) { // was while(IRDist > 800)
        degrees -= 2;
        move_servo(degrees);
        timer_waitMillis(100);
        IRDist = ir_read();
    }
    unsigned int end = degrees;

    int radial_width = start-end;
    int real_width = radialToRealC(radial_width, distance);

    if(smallestWidth > real_width && real_width>0) {
    	objCount ++;
    	lcd_printf("Object Count: %d", objCount);
        smallestWidth = real_width;
        smallestLocation = start;
        uart_sendStr("WIDTH: ");
        uart_sendNum(real_width);
        uart_sendStr("\n\r");
    }
}

void uart_sendNum(int num) {
	char str[15];
	sprintf(str, "%d", num);
	uart_sendStr(str);
}

int radialToRealC(double radial_width, int distanceToObject) {
    double radians = toRadians(radial_width);
    return (int)(2*distanceToObject*sin(radians));
}

double toRadians(double angle) {
    return angle * ((double) M_PI / 180);
}

void ir_timer_init(){
	//enable ADC 0 module on port D
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
	//enable clock for ADC
	SYSCTL_RCGCADC_R |= 0x1;
	//enable port D pin 0 to work as alternate functions
	GPIO_PORTB_AFSEL_R |= 0x01;
	//set pin to input - 0
	GPIO_PORTB_DEN_R &= 0b11111110;
	//disable analog isolation for the pin
	GPIO_PORTB_AMSEL_R |= 0x01;
	//initialize the port trigger source as processor (default)
	GPIO_PORTB_ADCCTL_R = 0x00;

	//disable SS0 sample sequencer to configure it
	ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN0;
	//initialize the ADC trigger source as processor (default)
	ADC0_EMUX_R = ADC_EMUX_EM0_PROCESSOR;
	//set 1st sample to use the AIN10 ADC pin
	ADC0_SSMUX0_R |= 0x000A;
	//enable raw interrupt
	ADC0_SSCTL0_R |= (ADC_SSCTL0_IE0 | ADC_SSCTL0_END0);
	//enable oversampling to average
	ADC0_SAC_R |= ADC_SAC_AVG_64X;
	//re-enable ADC0 SS0
	ADC0_ACTSS_R |= ADC_ACTSS_ASEN0;
}

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
}

void move_servo(degree){
	unsigned int out = degree*(133) + 10000;
//	if(direction == 1){
//		lcd_printf("%d inc",out);
//	}
//	else{
//		lcd_printf("%d dec",out);
//	}
	TIMER1_TBMATCHR_R = (320000 - out) & 0xFFFF;
	TIMER1_TBPMR_R = (320000 - out) >> 16;
}

 int ir_read(){
	//initiate SS1 conversion
	ADC0_PSSI_R=ADC_PSSI_SS0;
	//wait for ADC conversion to be complete
	while((ADC0_RIS_R & ADC_RIS_INR0) == 0){
	//wait
	}
	//grab result
	return ADC0_SSFIFO0_R;
 }

 unsigned sonar_read() {
 	send_pulse();
 	while(count != 2);
 	count = 0;
 	unsigned distance = ping_read();
 	return distance;
  }

  unsigned ping_read() {
  	unsigned elapsed_time = falling_time-rising_time;
  	int distance = time2dist(elapsed_time);
  	return distance;
  }

  unsigned time2dist(unsigned time) {
  	double interval = time*17;
  	int out = interval/1600;
  	return out;
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
