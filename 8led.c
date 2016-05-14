#include <stdio.h>
#include <stdlib.h>
#include "gpio.h"
#include "modes.h"

#define PIN_NUMBER 8
#define DELAY_DEFAULT 100
#define DELAY_MAX 2000
#define DELAY_MIN 10

int select_mode(void);
int get_delay();

int main(void) {
    //int pins[PIN_NUMBER] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    int pins[PIN_NUMBER] = { 7, 6, 5, 4, 3, 2, 1, 0 };
    int delay_value;
    int mode;

    setup_gpio(pins, PIN_NUMBER);

    mode = select_mode();
    switch (mode) {
        case 1:
            delay_value = get_delay();
            binary_counter(pins, PIN_NUMBER, delay_value);
            break;
        case 2:
            delay_value = get_delay();
            flowing_lights(pins, PIN_NUMBER, delay_value);
            break;
        default:
            printf("Mode does not exist.\n");
            return EXIT_FAILURE;
    }

    cleanup_gpio(pins, PIN_NUMBER);

    return EXIT_SUCCESS;
}

int select_mode(void) {
    char mode_string[80];
    int retval;

    printf(
        "Modes:\n"
        "\t1: binary counter\n"
        "\t2: flowing lights\n"
        "Select: "
    );
    retval = scanf("%[12]", mode_string);
    if (retval != 1) {
        return -1;
    } else {
        return atoi(mode_string);
    }
}

int get_delay() {
    int delay_value;
    int scanf_retval;

    printf("Delay [ms]: ");
    scanf_retval = scanf("%d", &delay_value);
    if (scanf_retval != 1) {
        printf("Input error!\n");
        return DELAY_DEFAULT;
    }

    if (delay_value > DELAY_MAX || delay_value < DELAY_MIN) {
        printf(
            "Delay value out of bounds: %d <= DELAY <= %d\n"
            "Using default delay: %d ms\n",
            DELAY_MIN, DELAY_MAX, DELAY_DEFAULT
        );
        delay_value = DELAY_DEFAULT;
    }

    return delay_value;
}

