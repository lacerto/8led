#ifndef GPIO_H
#define GPIO_H

/* Setup wiringPi and make the pins outputs. */
void setup_gpio         (int *pins, int pin_number,
                            int use_sys_mode,
                            int dry_run);

/* Set all pins to high level. */
void cleanup_gpio       (int *pins, int pin_number);

/* Set a single pin to low level. */
void pin_low            (int pin);

/* Set a single pin to high level. */
void pin_high           (int pin);

/* Set the mode of a pin to output. */
void set_pin_output     (int pin);

/* Set the mode of a pin to PWM output. */
void set_pin_pwm_output (int pin);

/* Set the PWM value for a pin.*/
void set_pwm_value      (int pin, int value);

/* Delay for ms milliseconds. */
void delay_ms           (int ms);

#endif
