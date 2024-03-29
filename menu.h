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
	int x_press;
	int y_press;
	int width;
	int height;
} menu_button_t;

extern menu_t menu;

extern char *all_categories[9];
extern char *categories[9];
extern int categories_length;

extern menu_button_t left_category_button;
extern menu_button_t right_category_button;
extern menu_button_t logout_menu_button;
extern menu_button_t lock_menu_button;
extern menu_button_t new_terminal_menu_button;

extern int up_line_y;
extern int bottom_line_y;

extern Pixmap menu_pixmap;
extern Pixmap icons_pixmap;

void menu_init(void);
void menu_clear(void);

void menu_show(void);
void menu_show_categories(void);
void menu_show_new_terminal_button(void);
void menu_show_logout_button(void);
void menu_show_lock_button(void);
void menu_show_sound(void);
void menu_show_brightness(void);
void menu_show_battery(void);

int menu_left_category_is_hover(int x, int y);
int menu_right_category_is_hover(int x, int y);
int menu_apps_is_hover(int x, int y);
int menu_new_terminal_is_hover(int x, int y);
int menu_logout_is_hover(int x, int y);
int menu_lock_is_hover(int x, int y);
int menu_sound_is_hover(int x, int y);
int menu_brightness_is_hover(int x, int y);

int menu_buttons_on_hover(int x, int y);
int menu_category_buttons_on_hover(int x, int y);

void menu_category_buttons_on_press(int category_button);
void menu_on_scroll(int button, int x, int y);

void menu_go_to_previous_category(void);
void menu_go_to_next_category(void);
#endif
