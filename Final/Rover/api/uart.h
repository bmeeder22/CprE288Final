/*
*
*   uart.h
*
*   Used to set up the RS232 connector and WIFI module
*   uses UART1 at 115200
*
*
*   @author Dane Larson
*   @date 07/18/2016
*/

#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

void uart_init(void);

void uart_sendChar(char data);

int uart_receive(void);

void uart_flush(void);

void uart_sendStr(const char *data);

void uart_sendBuffer(char* buffer, int length);

void UART1_Handler(void);

char* uart_receivePacket();

void uart_sendNum(int num);

void uart_sendStrNoNewline(const char *data);

#endif /* UART_H_ */
