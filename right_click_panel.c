#include <stdlib.h>
#include <string.h>
#include "right_click_panel.h"
#include "objects.h"
#include "wallpaper.h"
#include "menu.h"
#include "window.h"

panel_t panel;
int panel_showed = 0;
int buttons_length = 5;
char *button_titles[5] = { "Change wallpaper", "Shortcuts", "Themes", "Lock", "Quit" };
panel_button_t *buttons[5];

void right_click_panel_show(int x, int y) {
	int panel_width = 0;

	panel_showed = 1;

	for (int i = 0; i < buttons_length; i++) {
		int width = XTextWidth(font_struct, button_titles[i], (int) strlen(button_titles[i]));
		if (width > panel_width) {
			panel_width = width + 50;
		}
	}

	if (x + panel_width > screen_width) {
		x = screen_width - panel_width;
	}

	if (y + 40 * buttons_length > screen_height) {
		y = screen_height - 40 * buttons_length;
	}

	panel = (panel_t) {
		.x = x,
		.y = y,
		.width = panel_width,
		.height = 40 * buttons_length
	};

	XFillRectangle(display, window, gc_text_white, panel.x, panel.y, panel.width, panel.height);

	for (int i = 0; i < buttons_length; i++) {
		buttons[i] = (panel_button_t *) malloc(sizeof(panel_button_t));

		buttons[i]->label = button_titles[i];
		buttons[i]->x = panel.x;
		buttons[i]->y = panel.y + (40 * i);
		buttons[i]->width = panel.width;
		buttons[i]->height = 40;

		XDrawString(display, window, gc_text_black, panel.x + 5, panel.y + 24 + (40 * i), button_titles[i], (int) strlen(button_titles[i]));
	}
}

int right_click_panel_on_hover(int x, int y) {
	if (panel_showed == 1) {
		int button_hovered = -1;

		for (int i = 0; i < buttons_length; i++) {
			if (x >= buttons[i]->x && x <= buttons[i]->x + buttons[i]->width && y > buttons[i]->y && y < buttons[i]->y + buttons[i]->height) {
				button_hovered = i;
				XFillRectangle(display, window, gc_right_panel_hover, buttons[i]->x, buttons[i]->y, buttons[i]->width, buttons[i]->height);
				XDrawString(display, window, gc_text_white, panel.x + 5, panel.y + 24 + (40 * i), button_titles[i], (int) strlen(button_titles[i]));
			} else {
				XFillRectangle(display, window, gc_text_white, buttons[i]->x, buttons[i]->y, buttons[i]->width, buttons[i]->height);
				XDrawString(display, window, gc_text_black, panel.x + 5, panel.y + 24 + (40 * i), button_titles[i], (int) strlen(button_titles[i]));
			}
		}

		return button_hovered;
	}

	return -1;
}

int right_click_panel_on_press(int x, int y) {
	int button_pressed = right_click_panel_on_hover(x, y);
	
	switch (button_pressed) {
		case 0:
			wallpaper_open_window();
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			menu_clear();
			window_hide_all_visible();
			XDefineCursor(display, window, cursor);
			return 2;
		case 4:
			return 1;
	}

	return 0;
}

void right_click_panel_hide(void) {
	panel_showed = 0;

	XEvent e = { .type = Expose };
	XSendEvent(display, window, 0, ExposureMask, &e);
}
