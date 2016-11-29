/*
 * servo.h
 *
 *  Created on: Nov 8, 2016
 *      Author: nicks1
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inc/tm4c123gh6pm.h>

#include "Timer.h"

#define BIT0		0x01
#define BIT1		0x02
#define BIT2		0x04
#define BIT3		0x08
#define BIT4		0x10
#define BIT5		0x20
#define BIT6		0x40
#define BIT7		0x80

void servo_timer_init();
void move_servo(int degree);
unsigned int move_servo_return(int degree);



#endif /* SERVO_H_ */
