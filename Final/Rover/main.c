/*
 * main.c
 */
#include "api/WiFi.h"
#include "api/lcd.h"
#include "api/uart.h"
#include "api/Timer.h"
#include "api/servo.h"
#include "api/ir.h"
#include "api/sonar.h"
#include "api/sweep.h"
#include "api/button.h"

#include <string.h>
#include <math.h>

void init_all();
void handleCommand(char in);
void calibrateIR();
void calibrateServo();

int main(void) {
	init_all();

	while(1){
		char in = uart_receive();
		handleCommand(in);
	}
}

void handleCommand(char in) {
	if(in == 'f') {
		lcd_printf("%s", "FORWARD");
		uart_sendStr("FORWARD");
	}
	if(in == 'b') {
		lcd_printf("%s", "BACKWARDS");
		uart_sendStr("BACKWARDS");
	}
	if(in == 'l') {
		lcd_printf("%s", "LEFT");
		uart_sendStr("LEFT");
	}
	if(in == 'r') {
		lcd_printf("%s", "RIGHT");
		uart_sendStr("RIGHT");
	}
	if(in == 's') {
		uart_sendStr("start_sweep");
		sweep();
		uart_sendStr("end_sweep");
	}
	if(in == 'm') {
		move_servo(180);
		timer_waitMillis(800);
		move_servo(0);
	}
}

void init_all() {
	lcd_init();
    sonar_timer_init();
	ir_timer_init();
	servo_timer_init();
	uart_init();

	/****Calibrations*****/
//	calibrateServo();
//	calibrateIR();

	/*****WIFI***********/
//	WiFi_start("aaaaaaaa\0");
//	while(1) {}
}

void calibrateIR() {
	move_servo(90);
	while(1) {
		int reading = ir_read();
		lcd_printf("%d", reading);
		timer_waitMillis(500);
	}
}

void calibrateServo() {
	while(1) {
		lcd_printf("%d", move_servo_return(180));
		timer_waitMillis(1000);
		lcd_printf("%d", move_servo_return(90));
		timer_waitMillis(500);
		lcd_printf("%d", move_servo_return(0));
		timer_waitMillis(1000);
	}
}
