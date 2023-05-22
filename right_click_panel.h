#ifndef RIGHT_CLICK_PANEL_H
#define RIGHT_CLICK_PANEL_H
typedef struct panel_t {
	int x;
	int y;
	int width;
	int height;
} panel_t;

typedef struct panel_button_t {
	char *label;
	int x;
	int y;
	int width;
	int height;
} panel_button_t;
void right_click_panel_show(int x, int y);
int right_click_panel_on_hover(int x, int y);
int right_click_panel_on_press(int x, int y);
void right_click_panel_hide(void);
#endif