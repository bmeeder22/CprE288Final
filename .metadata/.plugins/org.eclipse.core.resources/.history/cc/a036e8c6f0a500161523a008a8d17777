/*
 * WiFi.c
 *
 *  Created on: Sep 25, 2016
 *      Author: ericm
 */

#include <string.h>

#include "WiFi.h"

#define COMMAND_PIN		0x04

#define COMMAND_START	0
#define COMMAND_STOP	1

#define RETVAL_SUCCESS	0

uint8_t _sendCommand(uint8_t command, uint8_t* param, uint8_t len);

int WiFi_start(char *psk) {
	//Initialize UART
	uart_init();

	//Initialize command pin for WiFi connection
	SYSCTL_RCGCGPIO_R |= BIT1; //Turn on GPIO for WiFi command mode
	GPIO_PORTB_DEN_R |= COMMAND_PIN; //Enable pin2
	GPIO_PORTB_DIR_R |= COMMAND_PIN;
	GPIO_PORTB_DATA_R &= ~COMMAND_PIN; //Clear command pin

	//Wait for pin to settle
	timer_waitMillis(1);

	timer_waitMillis(1);

	uint8_t response = _sendCommand(COMMAND_START, psk, strlen(psk) + 1);

	if(response != RETVAL_SUCCESS) {
		return -1;
	}
	else {
		return 0;
	}
}

int WiFi_stop() {
	return _sendCommand(COMMAND_STOP, NULL, 0);
}

uint8_t _sendCommand(uint8_t command, uint8_t* param, uint8_t len) {
	//Raise command flag
	GPIO_PORTB_DATA_R |= COMMAND_PIN;

	//Send WiFi command
	uart_sendChar(command);

	//Send parameter (if applicable)
	if(len > 0) {
		uart_sendBuffer(param, len);
	}

	//Wait for response from WiFi microcontroller
	int response = uart_receive();

	//Lower the command flag
	GPIO_PORTB_DATA_R &= ~COMMAND_PIN;

	return response;
}
