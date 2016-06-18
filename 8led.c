#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <menu.h>
#include "gpio.h"
#include "modes.h"

#define PIN_NUMBER 8
#define DELAY_DEFAULT 100
#define DELAY_MAX 2000
#define DELAY_MIN 10

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

char *menu_item_strings[] =
{
    "Binary counter",
    "Flowing lights",
    "Breathing LED",
    "Exit"
};

int process_selected_item(int *pins, int item_idx);
int select_mode(void);
int get_delay();

int main(void) {
    // Pins connected to the LEDs.
    int pins[PIN_NUMBER] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    //int pins[PIN_NUMBER] = { 7, 6, 5, 4, 3, 2, 1, 0 };

    ITEM **menu_items;
    ITEM *curr_item;
    MENU *main_menu;
    int item_count;
    int i;
    int process_events;
    int ch;
    int cursor_visibility;

    // Initialize ncurses.
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    cursor_visibility = curs_set(0);

    // Create menu items.
    item_count = ARRAY_SIZE(menu_item_strings);
    menu_items = (ITEM**) calloc(item_count + 1, sizeof(ITEM*));
    for (i=0; i<item_count; i++) {
        menu_items[i] = new_item(menu_item_strings[i], NULL);
    }
    menu_items[item_count] = (ITEM*) NULL;

    // Create and display the menu.
    main_menu = new_menu(menu_items);
    set_menu_mark(main_menu, "*");
    post_menu(main_menu);
    refresh();

    // Setup wiringPi and set all the pins outputs.
    setup_gpio(pins, PIN_NUMBER);

    process_events = 1;
    while (process_events) {
        ch = getch();
        switch (ch) {
            case KEY_DOWN:
                menu_driver(main_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(main_menu, REQ_UP_ITEM);
                break;
            case '\n': // normal enter key
            case KEY_ENTER: // enter key on the keypad
                unpost_menu(main_menu);
                clear();
                printw("Press C-c to return to menu.");
                refresh();

                curr_item = current_item(main_menu);
                process_events = process_selected_item(
                    pins, item_index(curr_item)
                );

                clear();
                post_menu(main_menu);
                refresh();
                break;
            case 'q':
            case 'Q':
                process_events = 0;
                break;
        }
    }

    // Switch off all LEDs.
    cleanup_gpio(pins, PIN_NUMBER);

    unpost_menu(main_menu);
    for (i=0; i<item_count + 1; i++) {
        free_item(menu_items[i]);
    }
    free_menu(main_menu);

    curs_set(cursor_visibility);
    endwin();

    return EXIT_SUCCESS;
}

int process_selected_item(int *pins, int item_idx) {
    int retval = 1;

    switch (item_idx) {
        case 0:
            binary_counter(pins, PIN_NUMBER, DELAY_DEFAULT);
            break;
        case 1:
            flowing_lights(pins, PIN_NUMBER, DELAY_DEFAULT);
            break;
        case 2:
            breathing_led();
            break;
        case 3:
            retval = 0;
            break;
    }

    return retval;
}

int get_delay() {
    int delay_value;
    int scanf_retval;

    printf("Delay [ms]: ");
    scanf_retval = scanf("%d", &delay_value);
    if (scanf_retval != 1) {
        printf("Input error!\nUsing default delay: %d ms\n", DELAY_DEFAULT);
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

