#include "sweep.h"

int degree;
int degrees = 180;
int objCount = 0;
int smallestLocation = 10000;
int smallestWidth = 10000;

void sweep() {
    while(degrees>0) {
        move_servo(degrees);
        degrees --;

        int IRDist = ir_read();
        int SonarDist = sonar_read();
        SonarDist = SonarDist/10;

        if(SonarDist < 85 && IRDist > 700) { // was if(SonarDist < 20 && SonarDist!=0)
        	lcd_printf("Sonar %d, IR %d", SonarDist, IRDist);
            handleObjectFound(SonarDist);
        }

        timer_waitMillis(50);
    }

    degrees = 180;
    objCount = 0;
    smallestLocation = 10000;
    smallestWidth = 10000;

    move_servo(90);
    timer_waitMillis(500);
}

void handleObjectFound(int distance) {
	timer_waitMillis(100);
    unsigned int start = degrees;
    int IRDist = ir_read();
    int IRStart = ir_read();
//    int distance = sonar_read()/10;

    while(IRDist > 700) { // was while(IRDist > 800)
        degrees -= 1;
        if(degrees < 0) break;

        move_servo(degrees);
        timer_waitMillis(100);
        IRDist = ir_read();
    }
    unsigned int end = degrees;

    int radial_width = start-end;
    int real_width = radialToRealC(radial_width, distance);

    if(real_width>2) {
    	distance = getAccurateDistance(radial_width);
    	real_width = radialToRealC(radial_width, distance);
    	objCount ++;
        smallestWidth = real_width;
        smallestLocation = start;

        uart_sendStrNoNewline("WIDTH:");
        uart_sendNum(real_width);
        uart_sendStrNoNewline(";LOCATION:");
        uart_sendNum(start);
        uart_sendStrNoNewline(";DISTANCE:");
        uart_sendNum(distance);
        uart_sendStrNoNewline("\n");
    }
    timer_waitMillis(100);
}

int getAccurateDistance(int radialWidth) {
	move_servo(degrees-radialWidth/2.0);
	timer_waitMillis(200);
	int SonarDist = sonar_read()/10.0;
	timer_waitMillis(50);
	move_servo(degrees);
	timer_waitMillis(200);
	return SonarDist;
}

int getIRReading() {
	int IR_1 = ir_read();
	int IR_2 = ir_read();
	int IR_3 = ir_read();
	int IR_4 = ir_read();

	return (IR_1+IR_2+IR_3+IR_4)/4.0;
}

int radialToRealC(double radial_width, int distanceToObject) {
    double radians = toRadians(radial_width);
//    return (int)(2*distanceToObject*sin(radians));

	return 2*distanceToObject*tan(radians/2.0);
}

double toRadians(double angle) {
    return angle * ((double) M_PI / 180);
}


