#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include <stdio.h>
#include "open_interface.h"

void turn_clockwise(oi_t *sensor, int degrees);

void turn_cclockwise(oi_t *sensor, int degrees);

void move_forward(oi_t *sensor, int millimeters);

void move_backwards(oi_t *sensor, int millimeters);

int checkObstacle(oi_t *sensor);

int checkCliff(oi_t *sensor);

int checkBlack(oi_t *sensor);

int move_forward_noChecks(oi_t *sensor, int distance);

int checkLineTurn(oi_t *sensor, int sum);

void handleLineFound(oi_t *sensor);

void turnUntilCenter(oi_t *sensor, int direction);

void finishHandleLine(oi_t *sensor, int direction);

void playSong();

#endif
