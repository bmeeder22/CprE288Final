#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <stdio.h>
#include "open_interface.h"

void turn_clockwise(oi_t *sensor, int degrees);

void turn_cclockwise(oi_t *sensor, int degrees);

void move_forward(oi_t *sensor, int millimeters);

void move_backwards(oi_t *sensor, int millimeters);

int checkLine(oi_t *sensor, int sum);

int checkObstacle(oi_t *sensor);

int checkCliff(oi_t *sensor);

#endif
