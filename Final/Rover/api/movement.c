#include "movement.h"
#include <math.h>
#include "uart.h"

int checkLine(oi_t *sensor){
	int tolerance = 2650;
	if (sensor->cliffLeftSignal > tolerance || sensor->cliffRightSignal > tolerance || sensor->cliffFrontLeftSignal > tolerance || sensor->cliffFrontRightSignal > tolerance)
		return 1;
	else
		return 0;
}

int checkObstacle(oi_t *sensor){
	if (sensor->bumpLeft || sensor->bumpRight)
		return 1;
	else
		return 0;
}

int checkCliff(oi_t *sensor){
	if (sensor->cliffLeft || sensor->cliffRight || sensor->cliffFrontLeft || sensor->cliffFrontRight)
		return 1;
	else
		return 0;
}

void move_forward(oi_t *sensor, int millimeters){
	int sum = 0;
	oi_setWheels(100, 100);
	while (sum < millimeters) {
		if (checkCliff(sensor)){
			move_backwards(sensor, 100);
			break;
		}

		if (checkObstacle(sensor)){
			move_backwards(sensor, 100);
			break;
		}

		if (checkLine(sensor)){
			move_backwards(sensor, 100);
			break;
		}

		oi_update(sensor);
		sum += sensor->distance;
	}
	oi_setWheels(0, 0);

}

void move_backwards(oi_t *sensor, int millimeters){
	int sum = 0;
	oi_setWheels(-100, -100);
	while (sum < millimeters) {
		oi_update(sensor);
		sum += abs(sensor->distance);
	}
	oi_setWheels(0, 0);

}

void turn_clockwise(oi_t *sensor, int degrees){
	int sum = 0;
	oi_setWheels(-100, 100);
	while(sum <= degrees){
		oi_update(sensor);
		sum += abs(sensor->angle);
	}

	oi_setWheels(0,0);
}

void turn_cclockwise(oi_t *sensor, int degrees){
	int sum = 0;
	oi_setWheels(100, -100);
	while(sum <= degrees){
		oi_update(sensor);
		sum += abs(sensor->angle);
	}

	oi_setWheels(0,0);
}
