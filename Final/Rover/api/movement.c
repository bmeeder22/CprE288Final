#include "movement.h"
#include <math.h>
#include "uart.h"

volatile int found = 0;
volatile int initialDistance = 0;
volatile int done = 0;
volatile int foundBlack = 0;
volatile int angle = 0;

int checkBlack(oi_t *sensor){
	int blackTolerance = 1100;

	if (sensor->cliffFrontLeftSignal < blackTolerance || sensor->cliffFrontRightSignal < blackTolerance) {
		if(move_forward_noChecks(sensor, 160)) return 2;

		oi_update(sensor);
		if(sensor->cliffFrontLeftSignal<blackTolerance || sensor->cliffFrontRightSignal<blackTolerance) {
			return 1;
		}
		return 80;
	}
	return 0;
}

int checkLineTurn(oi_t *sensor, int sum) {
	int tolerance = 2650;
	if(sensor->cliffLeftSignal > tolerance || sensor->cliffFrontLeftSignal>tolerance || sensor->cliffRightSignal > tolerance || sensor->cliffFrontRightSignal>tolerance) {
		if(sensor->cliffLeftSignal > tolerance || sensor->cliffFrontLeftSignal>tolerance)
			move_forward_noChecks(sensor, 20);
		handleLineFound(sensor);
		uart_sendStrNoNewline("SUM: ");
		uart_sendNum(sum);
		uart_sendStr("");
		return 1;
	}
	return 0;
}

void handleLineFound(oi_t *sensor) {
	int tolerance = 2650;
	if (sensor->cliffFrontLeftSignal > tolerance && sensor->cliffFrontRightSignal > tolerance) {
		turn_clockwise(sensor, 180);
        uart_sendStr("LINEPERP");
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
			uart_sendStr("LINELEFT");
		}
		else {
			//turn right
			turn_clockwise(sensor, 10);
			degrees += 10;
            uart_sendStr("LINERIGHT");
		}
	}
    uart_sendStrNoNewline("ANGLE: ");
    uart_sendNum(90-degrees);
    uart_sendStr("");
	finishHandleLine(sensor, direction);
}

void finishHandleLine(oi_t *sensor, int direction) {
	if(direction == 1) {
		turn_clockwise(sensor, 180);
	}
	else {
		turn_cclockwise(sensor, 180);
	}
	uart_sendStr("LINEFINISHED");
}

int checkObstacle(oi_t *sensor){
	if (sensor->bumpLeft)
		return 1;
	else if (sensor->bumpRight)
		return 2;
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

		if (checkObstacle(sensor) == 1){
			move_backwards(sensor, 100);
			uart_sendStr("OBSTACLELEFT");
			uart_sendNum(sum);
			break;
		}
		else if (checkObstacle(sensor) == 2){
			move_backwards(sensor, 100);
			uart_sendStr("OBSTACLERIGHT");
			uart_sendNum(sum);
			break;
		}

		if(checkLineTurn(sensor, sum)) {
			break;
		}

		int dotFound = checkBlack(sensor);

		if(dotFound != 0){
			if(dotFound == 1) {
				playSong();
				break;
			}
			if(dotFound == 2) {
				break;
			}
			else {
				sum+=80;
			}
		}

		oi_update(sensor);
		sum += sensor->distance;
	}
	oi_setWheels(0, 0);
}

void playSong() {
    uart_sendStr("WIN");
    oi_init(oi_alloc());
    unsigned char numNotes = 5;
    unsigned char notes[5] = {75,74,73,72,71};//{80,78,80,75,80,78,78,77,76,0,0,0,0,0,0,0};
    unsigned char duration[5] = {15,15,15,15,15};//{12,12,30,35,10,10,15,20,10,10,10,30,30,30,30,30};
    oi_loadSong(0,numNotes,notes,duration);
    oi_play_song(0);
}

int move_forward_noChecks(oi_t *sensor, int distance) {
	int sum = 0;
	oi_setWheels(100, 100);
	while(sum < distance) {
		if (checkCliff(sensor)){
			move_backwards(sensor, 100);
			uart_sendStr("CLIFF");
			uart_sendNum(sum);
			return 1;
		}
		oi_update(sensor);
		sum += sensor->distance;
	}
	oi_setWheels(0, 0);
	return 0;
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
//	degrees *= 0.961;
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
//	degrees *= 0.961;
	oi_setWheels(50, -50);
	while(sum <= degrees){
		oi_update(sensor);
		sum += abs(sensor->angle);
	}

	oi_setWheels(0,0);
}
