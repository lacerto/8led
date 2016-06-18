#include <stdio.h>
#include <wiringPi.h>
#include "gpio.h"

/* Setup wiringPi and make the pins outputs. */
void setup_gpio(int *pins, int pin_number, int use_sys_mode) {
    int i;

    // Either mode uses the Broadcom numbering.
    if (use_sys_mode) {
        // For the sys mode to work, the pins must be exported (gpio export).
        wiringPiSetupSys();
    } else {
        wiringPiSetupGpio();
    }

	// All pins are outputs
	for (i=0; i<pin_number; i++) {
	    //printf("Pin %d -> output\n", pins[i]);
	    pinMode(pins[i], OUTPUT);
	    digitalWrite(pins[i], HIGH);
	}
	//printf("Setup complete.\n");
}

/* Set all pins to high level. */
void cleanup_gpio(int *pins, int pin_number) {
    int i;

	for (i=0; i<pin_number; i++) {
	    digitalWrite(pins[i], HIGH);
	}

    //printf("Cleanup complete.\n");
}

/* Set a single pin to low level. */
void pin_low(int pin) {
    digitalWrite(pin, LOW);
}

/* Set a single pin to high level. */
void pin_high(int pin) {
    digitalWrite(pin, HIGH);
}

/* Set the mode of a pin to output. */
void set_pin_output(int pin) {
    pinMode(pin, OUTPUT);
}

/* Set the mode of a pin to PWM output. */
void set_pin_pwm_output (int pin) {
    pinMode(pin, PWM_OUTPUT);
}

/* Set the PWM value for a pin.
   The RPi has only one channel for hardware PWM output: PIN 1.
*/
void set_pwm_value(int pin, int value) {
    pwmWrite(pin, value);
}

/* Delay for ms milliseconds. */
void delay_ms(int ms) {
    delay(ms);
}
