#include <stdio.h>
#include <math.h>
#include <signal.h>
#include "gpio.h"
#include "modes.h"

#define INTRO_REPEAT 5

typedef void (*sighandler_t)(int);

static volatile char running = 1;

void sigint_handler(int signum) {
    printf("\nInterrupt signal caught.\n");
    running = 0;
}

void binary_counter(int *pins, int pin_number, int delay_value) {
    unsigned int counter = 0;
    int i;
    int pow2;
    int mask = ((int) pow(2, pin_number) - 1);
    sighandler_t original_handler;

    original_handler = signal(SIGINT, sigint_handler);

    printf("Press C-c to exit.\n");
    printf("Counting...\n");

    while (running) {
        for (i=0; i<pin_number; i++)
        {
            pow2 = (int) pow(2, i);
            if ((counter & pow2) == pow2)
            {
                pin_low(pins[i]);
            } else {
                pin_high(pins[i]);
            }
        }
        counter++;
        counter &= mask;
        delay_ms(delay_value);
        if (counter == mask) {
            blink_all(pins, pin_number, delay_value, 3);
        }
    }

    signal(SIGINT, original_handler);
}

void flowing_lights(int *pins, int pin_number, int delay_value) {
    int current_pin = 0;
    int direction = 1;
    sighandler_t original_handler;

    original_handler = signal(SIGINT, sigint_handler);

	blink_all(pins, pin_number, delay_value, INTRO_REPEAT);

    printf("Press C-c to exit.\n");
    printf("Looping...\n");

    while (running) {
       pin_low(pins[current_pin]); // LED on
       delay_ms(delay_value);
       pin_high(pins[current_pin]); // LED off

       current_pin += direction;
       if (current_pin == pin_number) {
           current_pin = pin_number - 2;
           direction = -1;
       }
       if (current_pin < 0) {
           current_pin = 1;
           direction = 1;
       }
    }

    signal(SIGINT, original_handler);
    printf("Loop exited.\n");

	blink_all(pins, pin_number, delay_value, INTRO_REPEAT);
}

void breathing_led(void) {
    int i = 0;
    int direction = 1;
    const int pwm_pin = 1;
    sighandler_t original_handler;

    original_handler = signal(SIGINT, sigint_handler);

    printf("Press C-c to exit.\n");
    printf("Looping...\n");

    set_pin_pwm_output(pwm_pin);

    while (running) {
        set_pwm_value(pwm_pin, i);
        delay_ms(5);
        i += direction;
        if (i>1024) {
            direction = -1;
            i = 1024 - direction;
        }
        if (i<0)
        {
            direction = 1;
            i = 0 + direction;
        }
    }

    set_pin_output(pwm_pin);

    signal(SIGINT, original_handler);
    printf("Loop exited.\n");
}

void blink_all(int *pins, int pin_number, int delay_value, int repeat) {
    int i;
    int j;

    printf("All LEDs are blinking %d times.\n", repeat);

    for (i=0; i<repeat; i++) {
        for (j=0; j<pin_number; j++) {
            pin_low(pins[j]); // LED on
        }

        delay_ms(delay_value);

        for (j=0; j<pin_number; j++) {
            pin_high(pins[j]); // LED off
        }

        delay_ms(delay_value);
    }
}

