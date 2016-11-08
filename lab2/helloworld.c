#include "lcd.h"

/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 */
int main (void) {

	char message[74] = "                   Microcontrollers are lots of fun!                    /0";

	char shown[21];
	lcd_init();
	int startPos = 0;
	while(1) {

		int shownIndex;
		for(shownIndex = 0; shownIndex<20; shownIndex++) {
			shown[shownIndex] = message[startPos+shownIndex];
		}
		shown[20] = '\0';
		//display shown on lcd
		lcd_printf(shown);
		timer_waitMillis(300);
		startPos++;

		if(startPos == 52) startPos = 0;
	}
    
	return 0;
}
