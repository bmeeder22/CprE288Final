/*
 * WiFi.h
 *
 *  Created on: Sep 25, 2016
 *      Author: ericm
 */

#ifndef WIFI_H_
#define WIFI_H_

#include <stdint.h>
//THE LINE BELOW SHOULD POINT TO YOUR UART HEADER FILE
#include "uart.h"

int WiFi_start(char psk[]);

int WiFi_stop();


#endif /* WIFI_H_ */
