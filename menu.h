#ifndef MENU_H
#define MENU_H
typedef struct menu_t {
    int x;
    int y;
    int width;
    int height;
    int category_index;
    int is_showed;
} menu_t;

typedef struct menu_button_t {
    int visible;
    int x;
    int y;
    int width;
    int height;
} menu_button_t;

extern menu_t menu;
extern char *categories[9];
extern menu_button_t left_category_button;
extern menu_button_t right_category_button;
extern menu_button_t logout_menu_button;
extern menu_button_t new_terminal_menu_button;

void menu_init(void);
void menu_show(void);
void menu_clear(void);
void menu_show_categories(void);
void menu_show_new_terminal_button(void);
void menu_show_logout_button(void);
int menu_buttons_on_hover(int x, int y);
int menu_category_buttons_on_hover(int x, int y);
void menu_category_buttons_on_press(int category_button);
void menu_go_to_previous_category(void);
void menu_go_to_next_category(void);
#endif
