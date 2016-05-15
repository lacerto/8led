#ifndef MODES_H
#define MODES_H

/* Shows a binary counter using the available LEDs. */
void binary_counter (int *pins, int pin_number, int delay_value);

/* Shows a flowing light. */
void flowing_lights (int *pins, int pin_number, int delay_value);

/* Gradually changes the brightness of a LED using PWM. */
void breathing_led  (void);

/* Switch all LEDs on and off. */
void blink_all      (int *pins, int pin_number, int delay_value, int repeat);

#endif
