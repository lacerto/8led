#include <stdio.h>
#include <wiringPi.h>
#include "gpio.h"

/* Setup wiringPi and make the pins outputs. */
void setup_gpio(int *pins, int pin_number) {
    int i;
    
   	wiringPiSetup();
    
	// All pins are outputs
	for (i=0; i<pin_number; i++) {
	    //printf("Pin %d -> output\n", pins[i]);
	    pinMode(pins[i], OUTPUT);
	}
	printf("Setup complete.\n");
}

/* Set all pins to high level. */
void cleanup_gpio(int *pins, int pin_number) {
    int i;
    
	for (i=0; i<pin_number; i++) {
	    digitalWrite(pins[i], HIGH);
	}

    printf("Cleanup complete.\n");
}

/* Set a single pin to low level. */
void pin_low(int pin) {
    digitalWrite(pin, LOW);
}

/* Set a single pin to high level. */
void pin_high(int pin) {
    digitalWrite(pin, HIGH);
}

/* Delay for ms milliseconds. */
void delay_ms(int ms) {
    delay(ms);
}
