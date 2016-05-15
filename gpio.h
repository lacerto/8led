#ifndef GPIO_H
#define GPIO_H

void setup_gpio         (int *pins, int pin_number);

void cleanup_gpio       (int *pins, int pin_number);

void pin_low            (int pin);

void pin_high           (int pin);

void set_pin_output     (int pin);

void set_pin_pwm_output (int pin);

void set_pwm_value      (int pin, int value);

void delay_ms           (int ms);

#endif
