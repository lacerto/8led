#include <stdio.h>
#include <wiringPi.h>
#include "gpio.h"

void set_pin_mode(int pin, int mode) {
    pinMode(pin, mode);
}

/* Setup wiringPi and make the pins outputs. */
void setup_gpio(int *pins, int pin_number) {
    int i;

   	wiringPiSetup();

	// All pins are outputs
	for (i=0; i<pin_number; i++) {
	    //printf("Pin %d -> output\n", pins[i]);
	    set_pin_mode(pins[i], OUTPUT);
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

void set_pin_output(int pin) {
    set_pin_mode(pin, OUTPUT);
}

void set_pin_pwm_output (int pin) {
    set_pin_mode(pin, PWM_OUTPUT);
}

/* Set the PWM value for a pine.
   The Pi has only one channel for hardware PWM output: PIN 1.
*/
void set_pwm_value(int pin, int value) {
    pwmWrite(pin, value);
}

/* Delay for ms milliseconds. */
void delay_ms(int ms) {
    delay(ms);
}
