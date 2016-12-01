/*
 * sweep.h
 *
 *  Created on: Nov 17, 2016
 *      Author: nicks1
 */

#ifndef API_SWEEP_H_
#define API_SWEEP_H_

#include "uart.h"
#include "lcd.h"
#include "Timer.h"
#include "servo.h"
#include "ir.h"
#include "sonar.h"
#include <string.h>
#include <math.h>

void sweep();
int radialToRealC(double radial_width, int distanceToObject);
double toRadians(double angle);
void handleObjectFound(int distance);
int getIRReading();
int getAccurateDistance(int radialWidth);

#define	M_PI		3.14159265358979323846

#endif /* API_SWEEP_H_ */
