#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <menu.h>
#include "gpio.h"
#include "modes.h"

#define PIN_NUMBER 8
#define DELAY_DEFAULT 200
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
void show_menu(MENU *menu);
void hide_menu(MENU *menu);
WINDOW *create_menu_windows(MENU *menu);
void free_menu_windows(MENU *menu);
void decorate_menu_window(WINDOW *win);
int get_delay();

int main(int argc, char **argv) {
    // Pins connected to the LEDs. Use the Broadcom numbering.
    int pins[PIN_NUMBER] = { 17, 18, 27, 22, 23, 24, 25, 4 };
    ITEM **menu_items;
    ITEM *curr_item;
    MENU *main_menu;
    WINDOW *main_menu_win;
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

    // Create and display the menu.
    main_menu = new_menu(menu_items);
    main_menu_win = create_menu_windows(main_menu);
    set_menu_mark(main_menu, "*");
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
                    hide_menu(main_menu);
                    process_events = process_selected_item(
                        pins, PIN_NUMBER, item_index(curr_item)
                    );
                    show_menu(main_menu);
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
    free_menu_windows(main_menu);
    free_menu(main_menu);

    curs_set(cursor_visibility);
    endwin();

    return EXIT_SUCCESS;
}

void show_menu(MENU *menu) {
    WINDOW *win = menu_win(menu);
    post_menu(menu);
    pos_menu_cursor(menu);
    decorate_menu_window(win);
    wrefresh(win);
}

void hide_menu(MENU *menu) {
    WINDOW *win = menu_win(menu);
    unpost_menu(menu);
    wclear(win);
    wrefresh(win);
}

WINDOW *create_menu_windows(MENU *menu) {
    WINDOW *main_menu_win;
    WINDOW *main_menu_subwin;
    int sub_cols;
    int sub_rows;
    int win_cols;
    int win_rows;
    int sub_x = 1;
    int sub_y = 3;
    int win_x;
    int win_y;

    scale_menu(menu, &sub_rows, &sub_cols);
    win_rows = sub_rows + 4; //4=upper + lower border + title + line under title
    win_cols = sub_cols + 2; //2=left + right border
    win_y = (LINES - win_rows) / 2;
    win_x = (COLS - win_cols) / 2;
    main_menu_win = newwin(win_rows, win_cols, win_y, win_x);
    decorate_menu_window(main_menu_win);
    keypad(main_menu_win, TRUE);
    main_menu_subwin = derwin(main_menu_win, sub_rows, sub_cols, sub_y, sub_x);
    set_menu_win(menu, main_menu_win);
    set_menu_sub(menu, main_menu_subwin);

    return main_menu_win;
}

void free_menu_windows(MENU *menu) {
    WINDOW *win;
    WINDOW *sub;

    win = menu_win(menu);
    sub = menu_sub(menu);
    delwin(sub);
    delwin(win);
}

void decorate_menu_window(WINDOW *win) {
    int rows;
    int cols;
    char *title = "MENU";

    getmaxyx(win, rows, cols);
    box(win, 0, 0);
    mvwprintw(win, 1, (cols - strlen(title)) / 2, title);
	mvwaddch(win, 2, 0, ACS_LTEE);
	mvwhline(win, 2, 1, ACS_HLINE, cols - 2);
	mvwaddch(win, 2, cols - 1, ACS_RTEE);
}

int process_selected_item(int *pins, int pin_number, int item_idx) {
    int retval = 1;
    int delay;

    switch (item_idx) {
        case 0:
            delay = get_delay();
            binary_counter(pins, pin_number, delay);
            break;
        case 1:
            delay = get_delay();
            flowing_lights(pins, pin_number, delay);
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
    WINDOW *win;
    WINDOW *sub;
    int y;
    int x;
    int rows = 3; // borders + 1 input line
    int cols;
    int delay_value;
    int scanf_retval;
    char *label = "Delay [ms]: ";
    int cursor_visibility;

    cols = strlen(label) + 8;
    y = (LINES - rows) / 2;
    x = (COLS - cols) / 2;
    win = newwin(rows, cols, y, x);
    sub = derwin(win, 1, 5, 1, strlen(label) + 1);
    keypad(win, TRUE);
    echo();
    cursor_visibility = curs_set(1); // cursor visible
    box(win, 0, 0);
    mvwprintw(win, 1, 1, label);
    wrefresh(win);
    scanf_retval = wscanw(sub, "%d", &delay_value);
    if (scanf_retval != 1) {
        mvprintw(LINES-1, 0, "Input error! Using default delay: %d ms", DELAY_DEFAULT);
        getch();
        clear();
        refresh();
        delay_value = DELAY_DEFAULT;
    }

    if (delay_value > DELAY_MAX || delay_value < DELAY_MIN) {
        mvprintw(
            LINES-1, 0,
            "Delay value out of bounds: %d <= DELAY <= %d  "
            "Using default delay: %d ms",
            DELAY_MIN, DELAY_MAX, DELAY_DEFAULT
        );
        getch();
        clear();
        refresh();
        delay_value = DELAY_DEFAULT;
    }

    wclear(win);
    wrefresh(win);
    delwin(sub);
    delwin(win);
    noecho();
    curs_set(cursor_visibility); // restore cursor

    return delay_value;
}

