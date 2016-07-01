#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <ncurses.h>
#include "gpio.h"
#include "modes.h"

#define BLINK_REPEAT 3

typedef void (*sighandler_t)(int);

static volatile sig_atomic_t running = 1;

/* Interrupt signal handler. */
void sigint_handler(int signum) {
    //printf("\nInterrupt signal caught.\n");
    running = 0;
}

/*
  Shows a binary counter using the available LEDs.
  Counts from 0 to 2^(LED count)-1.
*/
void binary_counter(int *pins, int pin_number, int delay_value) {
    unsigned int counter = 0;
    int i;
    int pow2;
    // Max value that can be displayed.
    int mask = ((int) pow(2, pin_number) - 1);
    struct sigaction new_action;
    struct sigaction old_action;
    WINDOW *win;

    // Set the interrupt signal handler. Save the original handler.
    new_action.sa_handler = sigint_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, &new_action, &old_action);

    //printf("Press C-c to exit.\n");
    //printf("Counting...\n");

    win = newwin(10, 32, 7, 24);
    box(win, 0, 0);
    mvwprintw(win, 0, (32-18)/2, "> BINARY COUNTER <");
    mvwprintw(win, 2, 2, "Press C-c to return to menu.");
    mvwprintw(win, 4, 8, "1");
    mvwprintw(win, 5, 8, "2 6 3 1");
    mvwprintw(win, 6, 8, "8 4 2 6 8 4 2 1");
    wrefresh(win);

    while (running) {
        // Light only those LEDs that correspond to the
        // binary value of counter.
        for (i=0; i<pin_number; i++)
        {
            pow2 = (int) pow(2, i);
            if ((counter & pow2) == pow2)
            {
                pin_low(pins[i]);
                wattron(win, A_REVERSE);
                mvwaddch(win, 7, 22-i*2, ' ');
                wattroff(win, A_REVERSE);
                wrefresh(win);
            } else {
                pin_high(pins[i]);
            }
        }
        delay_ms(delay_value);
        // Max value reached, flash all LEDs a few times.
        if (counter == mask) {
            blink_all(pins, pin_number, delay_value, 3);
        }
        counter++;
        counter &= mask;
        mvwhline(win, 7, 8, ' ', 16);
    }
    running = 1;

    // Reset the LEDs.
	for (i=0; i<pin_number; i++) {
	    pin_high(pins[i]);
	}

    wclear(win);
    wrefresh(win);

    // Restore the original interrupt handler.
    sigaction(SIGINT, &old_action, NULL);
}

/* Shows a flowing light. */
void flowing_lights(int *pins, int pin_number, int delay_value) {
    int current_pin = 0;
    int direction = 1;
    struct sigaction new_action;
    struct sigaction old_action;

    // Set the interrupt signal handler. Save the original handler.
    new_action.sa_handler = sigint_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, &new_action, &old_action);

	blink_all(pins, pin_number, delay_value, BLINK_REPEAT);

    //printf("Press C-c to exit.\n");
    //printf("Looping...\n");

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
    running = 1;

    // Restore the original interrupt handler.
    sigaction(SIGINT, &old_action, NULL);
}

/*
    Gradually changes the brightness of a LED using PWM.
    This mode does not work when using the sys mode of wiringPi
    (sys/class/gpio interface). See the wiringPi documentation
    of pwmWrite().
*/
void breathing_led(void) {
    int i = 0;
    int direction = 1;
    // The RPi has hardware PWM support only for pin 1 (wPi) / 18 (BCM).
    const int pwm_pin = 18; // use the Broadcom GPIO pin number (see setup_gpio)
    struct sigaction new_action;
    struct sigaction old_action;

    // Set the interrupt signal handler. Save the original handler.
    new_action.sa_handler = sigint_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, &new_action, &old_action);

    //printf("Press C-c to exit.\n");
    //printf("Looping...\n");

    // The pin has to be configured as PWM output.
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
    running = 1;

    // Set the mode of the pin to normal output.
    set_pin_output(pwm_pin);

    // Restore the original interrupt handler.
    sigaction(SIGINT, &old_action, NULL);
}

/* Switch all LEDs on and off. */
void blink_all(int *pins, int pin_number, int delay_value, int repeat) {
    int i;
    int j;

    //printf("All LEDs are blinking %d times.\n", repeat);

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

