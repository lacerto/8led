#ifndef GPIO_H
#define GPIO_H

void setup_gpio     (int *pins, int pin_number);

void cleanup_gpio   (int *pins, int pin_number);

void pin_low        (int pin);

void pin_high       (int pin);

void delay_ms       (int ms);

#endif
