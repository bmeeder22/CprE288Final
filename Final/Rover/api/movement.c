#include "movement.h"
#include <math.h>
#include "uart.h"

volatile int found = 0;
volatile int initialDistance = 0;
volatile int done = 0;
volatile int foundBlack = 0;
volatile int angle = 0;

int checkBlack(oi_t *sensor, int sum){
	int tolerance = 1400;
	if (sensor->cliffLeftSignal < tolerance || sensor->cliffRightSignal < tolerance || sensor->cliffFrontLeftSignal < tolerance || sensor->cliffFrontRightSignal < tolerance && !done && initialDistance == 0 && !foundBlack ){
		initialDistance = sum;
		foundBlack = 1;
		return 0;
	}
	else if (foundBlack && !done && initialDistance > 0){
//		lcd_printf("%d", sum - initialDistance);
		if (sum - initialDistance >= 90){
			done = 1;
			initialDistance = 0;
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}

int checkLine(oi_t *sensor, int sum){
	int tolerance = 2650;
	if (sensor->cliffLeftSignal > tolerance || sensor->cliffRightSignal > tolerance || sensor->cliffFrontLeftSignal > tolerance || sensor->cliffFrontRightSignal > tolerance){
		timer_waitMillis(150);
		oi_update(sensor);
		if (sensor->cliffFrontLeftSignal > tolerance && sensor->cliffFrontRightSignal > tolerance){
			move_backwards(sensor, 200);
			turn_clockwise(sensor, 90);
			angle = 90;
			uart_sendStr("LINELEFT");
			uart_sendStrNoNewline("SUM:");
			uart_sendNum(sum);
		}
		else{
			if (!found){
				if (sensor->cliffLeftSignal > tolerance || sensor->cliffFrontLeftSignal > tolerance){
					move_backwards(sensor, 200);
					turn_cclockwise(sensor, 90);
					found = 1;
					uart_sendStr("LINELEFT");
					uart_sendStrNoNewline("SUM:");
					uart_sendNum(sum);
				}
				else if (sensor->cliffRightSignal > tolerance || sensor->cliffFrontRightSignal > tolerance){
					move_backwards(sensor, 200);
					turn_clockwise(sensor, 90);
					found = 1;
					uart_sendStr("LINERIGHT");
					uart_sendStrNoNewline("SUM:");
					uart_sendNum(sum);
				}
				move_forward_for_angle(sensor);
			}
			else if (found){
				found = 0;
				int temp = sum;
				if (sensor->cliffLeftSignal > tolerance || sensor->cliffFrontLeftSignal > tolerance){
					move_backwards(sensor, sum);
					angle = (int) (atan( (double) temp/200.0) * 180 / 3.14 + 90);
					turn_cclockwise(sensor, angle);
				}
				else if (sensor->cliffRightSignal > tolerance || sensor->cliffFrontRightSignal > tolerance){
					move_backwards(sensor, sum);
					angle = (int) (atan( (double) temp/200.0) * 180 / 3.14 + 90);
					turn_clockwise(sensor, angle);
				}
			}
			return 1;
		}
	}
	return 0;

}

int checkLineTurn(oi_t *sensor, int sum) {
	int tolerance = 2650;
	if(sensor->cliffLeftSignal > tolerance || sensor->cliffFrontLeftSignal>tolerance || sensor->cliffRightSignal > tolerance || sensor->cliffFrontRightSignal>tolerance) {
		if(sensor->cliffLeftSignal > tolerance || sensor->cliffFrontLeftSignal>tolerance)
			move_forward_for_angle(sensor);
		handleLineFound(sensor, sum);
		return 1;
	}
	return 0;
}

void handleLineFound(oi_t *sensor, int sum) {
	int tolerance = 2650;
	if (sensor->cliffFrontLeftSignal > tolerance && sensor->cliffFrontRightSignal > tolerance) {
		turn_clockwise(sensor, 180);
		return;
	}

	int direction = 0;

	//hit left
	if(sensor->cliffLeftSignal > tolerance || sensor->cliffFrontLeftSignal>tolerance) {
		direction = 1;
	}
	//hit right
	else if(sensor->cliffRightSignal > tolerance || sensor->cliffFrontRightSignal>tolerance) {
		direction = 0;
	}
	turnUntilCenter(sensor, direction);
}

void turnUntilCenter(oi_t *sensor, int direction) {
	int degrees = 0;
	int tolerance = 2650;
	while(!(sensor->cliffFrontLeftSignal > tolerance) || !(sensor->cliffFrontRightSignal > tolerance)) {
		if(direction == 1) {
			//turn left
			turn_cclockwise(sensor, 10);
			degrees += 10;
		}
		else {
			//turn right
			turn_clockwise(sensor, 10);
			degrees += 10;
		}
	}
	finishHandleLine(sensor, direction);
}

void finishHandleLine(oi_t *sensor, int direction) {
	if(direction == 1) {
		turn_clockwise(sensor, 167);
	}
	else {
		turn_cclockwise(sensor, 167);
	}
	uart_sendStr("LINEFINISHED");
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
			uart_sendNum(sum);
			break;
		}

		if (checkObstacle(sensor)){
			move_backwards(sensor, 100);
			uart_sendStr("OBSTACLE");
			uart_sendNum(sum);
			break;
		}

		//OLD checkLine
//		checkLine(sensor, sum);
//
//		if(angle != 0) {
//			uart_sendStrNoNewline("ANGLE:");
//			uart_sendNum(angle-90);
//			uart_sendStr("\n");
//			angle = 0;
//			break;
//		}

		//NEW CHECKLINE
		if(checkLineTurn(sensor, sum)) {
			break;
		}

		if(checkBlack(sensor, sum) && done){
			uart_sendStr("WIN");
			oi_init(oi_alloc());
			unsigned char numNotes = 5;
			unsigned char notes[5] = {75,74,73,72,71};//{80,78,80,75,80,78,78,77,76,0,0,0,0,0,0,0};
			unsigned char duration[5] = {15,15,15,15,15};//{12,12,30,35,10,10,15,20,10,10,10,30,30,30,30,30};
			oi_loadSong(0,numNotes,notes,duration);
			oi_play_song(0);
			break;
		}

		oi_update(sensor);
		sum += sensor->distance;
	}
	oi_setWheels(0, 0);
}

void move_forward_for_angle(oi_t *sensor){
	int sum = 0;
	oi_setWheels(100, 100);
	while(sum < 50){
//		if(checkLine(sensor, sum))
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
	if(degrees > 180) return;
	degrees = 1.164*degrees - 3.782;
	lcd_printf("%d", sensor->angle);
	while(sum <= degrees){
		oi_update(sensor);
		sum += abs(sensor->angle);
	}
	lcd_printf("%d, %d", sensor->angle, sum);


	oi_setWheels(0,0);
}

void turn_cclockwise(oi_t *sensor, int degrees){
	int sum = 0;
	degrees = 1.164*degrees - 3.782;
	oi_setWheels(50, -50);
	while(sum <= degrees){
		oi_update(sensor);
		sum += abs(sensor->angle);
	}

	oi_setWheels(0,0);
}
