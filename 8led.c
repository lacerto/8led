#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

int process_selected_item(int *pins, int pin_number, int item_idx);
int check_pins_exported(int *pins, int pin_number);
int get_delay();

int main(int argc, char **argv) {
    // Pins connected to the LEDs. Use the Broadcom numbering.
    int pins[PIN_NUMBER] = { 17, 18, 27, 22, 23, 24, 25, 4 };
    ITEM **menu_items;
    ITEM *curr_item;
    MENU *main_menu;
    WINDOW *main_menu_win;
    WINDOW *main_menu_subwin;
    WINDOW *msg_win;
    int item_count;
    int i;
    int process_events;
    int ch;
    int cursor_visibility;
    int use_sys_mode = 0;
    uid_t euid;

    opterr = 0;
    while ((ch = getopt(argc, argv, "s")) != -1) {
        if (ch == 's') {
            use_sys_mode = 1;
        }
    }

    if (!use_sys_mode) {
        euid = geteuid();
        if (euid != 0) {
            printf("Without the -s option, this program "
                    "must be run with root privileges.\n");
            return EXIT_FAILURE;
        }
    } else {
        if (check_pins_exported(pins, PIN_NUMBER) == -1) {
            printf("Please export the pins first.\n");
            return EXIT_FAILURE;
        }
    }

    // Setup wiringPi and set all the pins outputs.
    setup_gpio(pins, PIN_NUMBER, use_sys_mode);

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

    if (use_sys_mode) {
        // PWM does not work in sys mode (see pwmWrite() documentation),
        // so make the breathing led menu item not selectable.
        item_opts_off(menu_items[2], O_SELECTABLE);
    }

    // Create windows for the menu.
    main_menu_win = newwin(8, 18, 8, 31);
    box(main_menu_win, 0, 0);
    mvwprintw(main_menu_win, 1, 7, "MENU");
	mvwaddch(main_menu_win, 2, 0, ACS_LTEE);
	mvwhline(main_menu_win, 2, 1, ACS_HLINE, 16);
	mvwaddch(main_menu_win, 2, 17, ACS_RTEE);
    keypad(main_menu_win, TRUE);
    main_menu_subwin = derwin(main_menu_win, 4, 16, 3, 1);

    // Create and display the menu.
    main_menu = new_menu(menu_items);
    set_menu_mark(main_menu, "*");
    set_menu_win(main_menu, main_menu_win);
    set_menu_sub(main_menu, main_menu_subwin);
    post_menu(main_menu);
    wrefresh(main_menu_win);

    process_events = 1;
    while (process_events) {
        ch = wgetch(main_menu_win);
        switch (ch) {
            case KEY_DOWN:
                menu_driver(main_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(main_menu, REQ_UP_ITEM);
                break;
            case '\n': // normal enter key
            case KEY_ENTER: // enter key on the keypad
                curr_item = current_item(main_menu);

                // Process only selectable items.
                if ((item_opts(curr_item) & O_SELECTABLE) == O_SELECTABLE) {
                    msg_win = newwin(3, 31, 5, 24);
                    box(msg_win, 0, 0);
                    mvwprintw(msg_win, 1, 2, "Press C-c to return to menu.");
                    wrefresh(msg_win);

                    process_events = process_selected_item(
                        pins, PIN_NUMBER, item_index(curr_item)
                    );

                    wclear(msg_win);
                    wrefresh(msg_win);
                    delwin(msg_win);
                    pos_menu_cursor(main_menu);
                }
                break;
            case 'q':
            case 'Q':
                process_events = 0;
                break;
        }
        wrefresh(main_menu_win);
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

int process_selected_item(int *pins, int pin_number, int item_idx) {
    int retval = 1;

    switch (item_idx) {
        case 0:
            binary_counter(pins, pin_number, DELAY_DEFAULT);
            break;
        case 1:
            flowing_lights(pins, pin_number, DELAY_DEFAULT);
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

int check_pins_exported(int *pins, int pin_number) {
    const char *path = "/sys/class/gpio/gpio";
    char pin_name[30];
    int i;
    int file_exists;
    int retval = 0;

    for (i=0; i<PIN_NUMBER; i++) {
        sprintf(pin_name, "%s%d", path, pins[i]);
        printf("Checking pin: %s", pin_name);
        file_exists = access(pin_name, F_OK);
        if (file_exists == -1) {
            printf("\t-> not exported!\n");
            retval = -1;
            break;
        } else {
            printf("\t-> OK\n");
        }
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

