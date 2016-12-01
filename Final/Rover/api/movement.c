#include "movement.h"
#include <math.h>
#include "uart.h"

volatile int found = 0;
volatile int initialDistance = 0;

int checkLine(oi_t *sensor, int sum){
	int tolerance = 2650;
	if (sensor->cliffLeftSignal > tolerance || sensor->cliffRightSignal > tolerance || sensor->cliffFrontLeftSignal > tolerance || sensor->cliffFrontRightSignal > tolerance){
		if (!found){
			if (sensor->cliffLeftSignal > tolerance || sensor->cliffFrontLeftSignal > tolerance){
				move_backwards(sensor, 200);
				turn_cclockwise(sensor, 90);
				found = 1;
				uart_sendStr("LINELEFT");
			}
			else if (sensor->cliffRightSignal > tolerance || sensor->cliffFrontRightSignal > tolerance){
				move_backwards(sensor, 200);
				turn_clockwise(sensor, 90);
				found = 1;
				uart_sendStr("LINERIGHT");
			}
			move_forward(sensor, 2000);
		}
		else if (found){
			found = 0;
			int temp = sum;
			if (sensor->cliffLeftSignal > tolerance || sensor->cliffFrontLeftSignal > tolerance){
				move_backwards(sensor, sum);
	//			lcd_printf("%i sum: %i", (int) (atan((double) temp/200.0) * 180 / 3.14 + 90), temp);
				turn_cclockwise(sensor, (int) (atan( (double) temp/200.0) * 180 / 3.14 + 90));
			}
			else if (sensor->cliffRightSignal > tolerance || sensor->cliffFrontRightSignal > tolerance){
				move_backwards(sensor, sum);
	//			lcd_printf("%i sum: %i", (int) (atan((double) temp/200.0) * 180 / 3.14 + 90), temp);
				turn_clockwise(sensor, (int) (atan( (double) temp/200.0) * 180 / 3.14 + 90));
			}
		}
	}
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
			uart_sendStr("CLIFF");
			break;
		}

		if (checkObstacle(sensor)){
			move_backwards(sensor, 100);
			uart_sendStr("OBSTACLE");
			break;
		}

		if (checkLine(sensor, sum)){
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
	oi_setWheels(-50, 50);
	while(sum <= degrees){
		oi_update(sensor);
		sum += abs(sensor->angle);
	}

	oi_setWheels(0,0);
}

void turn_cclockwise(oi_t *sensor, int degrees){
	int sum = 0;
	oi_setWheels(50, -50);
	while(sum <= degrees){
		oi_update(sensor);
		sum += abs(sensor->angle);
	}

	oi_setWheels(0,0);
}
