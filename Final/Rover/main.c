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
#include "api/movement.h"

#include <string.h>
#include <math.h>

void init_all();
void handleCommand(char* in);
void calibrateIR();
void calibrateServo();
int parseInt(char hundreths, char tenths, char ones);

oi_t *sensor;

int main(void) {
	init_all();

	while(1){
		char* in = uart_receivePacket();
		handleCommand(in);
	}
}

void handleCommand(char* in) {
	if(in[0] == 'f') {
		int number = parseInt(in[1],in[2],in[3]);
		move_forward(sensor, number);
		uart_sendStr("EndMove");
	}
	if(in[0] == 'b') {
		int number = parseInt(in[1],in[2],in[3]);
		move_backwards(sensor, number);
		uart_sendStr("EndMove");
	}
	if(in[0] == 'l') {
		int number = parseInt(in[1],in[2],in[3]);
		turn_cclockwise(sensor, number);
		uart_sendStr("EndMove");
	}
	if(in[0] == 'r') {
		int number = parseInt(in[1],in[2],in[3]);
		turn_clockwise(sensor, number);
		uart_sendStr("EndMove");
	}
	if(in[0] == 's') {
		uart_sendStr("start_sweep");
		sweep();
		uart_sendStr("end_sweep");
	}
	if(in[0] == 'm') {
		int number = parseInt(in[1],in[2],in[3]);
	}
}

int parseInt(char hundreths, char tenths, char ones) {
	int hundred = hundreths - '0';
	int ten = tenths - '0';
	int one = ones - '0';
	return hundred*100+ten*10+one;
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

	sensor = oi_alloc();
	oi_init(sensor);
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
