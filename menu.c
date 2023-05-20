#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <X11/Xlib.h>
#include "objects.h"
#include "menu.h"
#include "icon.h"
#include "utils.h"
#include "sound.h"
#include "brightness.h"
#include "battery.h"

menu_t menu;

char *all_categories[9] = { "Games", "System", "Graphics", "Development", "Network", "Multimedia", "Office", "Settings", "Other" };
char *categories[9];
int categories_length = 0;

menu_button_t left_category_button;
menu_button_t right_category_button;
menu_button_t logout_menu_button;
menu_button_t lock_menu_button;
menu_button_t new_terminal_menu_button;

int up_line_y = 0;
int bottom_line_y = 0;

Pixmap menu_pixmap;
Pixmap icons_pixmap;

icon_t icon_sound;
icon_t icon_brightness;

void menu_init(void) {
	int width = (int) (screen_width * 0.70);
	int height = (int) (screen_height * 0.70);
	int x = (screen_width - width) / 2;
	int y = (screen_height - height) / 2;

	menu = (menu_t) { x, y, width, height, -1, 0 };
	up_line_y = 80;
	bottom_line_y = menu.height - 60;

	menu_pixmap = XCreatePixmap(display, window, width, height, screen_depth);
	icons_pixmap = XCreatePixmap(display, window, width, height - 141, screen_depth);
}

void menu_show(void) {
	XClearArea(display, window, menu.x, menu.y, menu.width, menu.height, 0);

	XFillRectangle(display, menu_pixmap, gc_menu, 0, 0, menu.width, menu.height);
	XFillRectangle(display, icons_pixmap, gc_menu, 0, 0, menu.width, menu.height - 141);

	menu.is_showed = 1;

	icons_draw();
	XDrawLine(display, menu_pixmap, gc_text_white, 0, up_line_y, menu.width, up_line_y);
	menu_show_categories();
	XDrawLine(display, menu_pixmap, gc_text_white, 0, bottom_line_y, menu.width, bottom_line_y);
	menu_show_new_terminal_button();
	menu_show_logout_button();
	menu_show_lock_button();
	menu_show_sound();
	menu_show_brightness();
	menu_show_battery();

	XCopyArea(display, icons_pixmap, menu_pixmap, XDefaultGCOfScreen(screen), 0, 0, menu.width, menu.height - 1, 0, up_line_y + 1);
	XCopyArea(display, menu_pixmap, window, XDefaultGCOfScreen(screen), 0, 0, menu.width, menu.height, menu.x, menu.y);
}

void menu_clear(void) {
	XClearArea(display, window, menu.x - 1, menu.y - 1, menu.width + 2, menu.height + 2, 1);
	menu.is_showed = 0;
}

void menu_show_categories(void) {
	char *category_name = malloc(sizeof(char) * 20);
	if (menu.category_index == -1) {
		strcpy(category_name, "All categories");
	} else {
		strcpy(category_name, categories[menu.category_index]);
	}

	left_category_button.visible = 0;
	right_category_button.visible = 0;
	int width;
	int height = 20;

	width = XTextWidth(font_struct, category_name, (int) strlen(category_name));
	int x = (menu.width - width) / 2;

	XDrawString(display, menu_pixmap, gc_text_white, x, 50, category_name, (int) strlen(category_name));

	if (menu.category_index == 0) {
		width = XTextWidth(font_struct, "< All categories", 16);
		left_category_button = (menu_button_t) { .visible = 1, .x = 40, .y = 25, .x_press = menu.x + 40, .y_press = menu.y + 25, .width = width + 20, .height = height + 20 };

		XFillRectangle(display, menu_pixmap, gc_category_button, left_category_button.x, left_category_button.y, left_category_button.width, left_category_button.height);
		XDrawString(display, menu_pixmap, gc_text_white, 50, 50, "< All categories", 16);
	}

	if (menu.category_index > 0) {
		strcpy(category_name, "< ");
		strcat(category_name, categories[menu.category_index - 1]);

		width = XTextWidth(font_struct, category_name, (int) strlen(category_name));
		left_category_button = (menu_button_t) { .visible = 1, .x = 40, .y = 25, .x_press = menu.x + 40, .y_press = menu.y + 25, .width = width + 20, .height = height + 20};

		XFillRectangle(display, menu_pixmap, gc_category_button, left_category_button.x, left_category_button.y, left_category_button.width, left_category_button.height);
		XDrawString(display, menu_pixmap, gc_text_white, 50, 50, category_name, (int) strlen(category_name));
	}

	if (menu.category_index < categories_length - 1) {
		strcpy(category_name, categories[menu.category_index + 1]);
		strcat(category_name, " >");

		width = XTextWidth(font_struct, category_name, (int) strlen(category_name));
		right_category_button = (menu_button_t) { .visible = 1, .x = menu.width - width - 60, .y = 25, .x_press = menu.x + menu.width - width - 60, .y_press = menu.y + 25, .width = width + 20, .height = height + 20 };

		XFillRectangle(display, menu_pixmap, gc_category_button, right_category_button.x, right_category_button.y, right_category_button.width, right_category_button.height);
		XDrawString(display, menu_pixmap, gc_text_white, menu.width - width - 50, 50, category_name, (int) strlen(category_name));
	}

	free(category_name);
}

void menu_show_new_terminal_button(void) {
	char *new_terminal_path = utils_get(UTILS_NEW_TERMINAL);
	Pixmap pixmap = icon_get_pixmap(new_terminal_path, 40, 40);
	free(new_terminal_path);

	const int x = 40;
	const int y = menu.height - 50;
	XCopyArea(display, pixmap, menu_pixmap, XDefaultGCOfScreen(screen), 0, 0, 40, 40, x, y);

	new_terminal_menu_button = (menu_button_t) { .visible = 1, .x = x, .y = y, .x_press = menu.x + x, .y_press = menu.y + y, .width = 40, .height = 40 };
}

void menu_show_logout_button(void) {	
	char *logout_path = utils_get(UTILS_LOGOUT);
	Pixmap pixmap = icon_get_pixmap(logout_path, 40, 40);
	free(logout_path);

	const int x = menu.width - 70;
	const int y = menu.height - 50;
	XCopyArea(display, pixmap, menu_pixmap, XDefaultGCOfScreen(screen), 0, 0, 40, 40, x, y);

	logout_menu_button = (menu_button_t) { .visible = 1, .x = x, .y = y, .x_press = menu.x + x, .y_press = menu.y + y, .width = 40, .height = 40 };
}

void menu_show_lock_button(void) {	
	char *lock_path = utils_get(UTILS_LOCK);
	Pixmap pixmap = icon_get_pixmap(lock_path, 40, 40);
	free(lock_path);

	const int x = menu.width - 120;
	const int y = menu.height - 50;
	XCopyArea(display, pixmap, menu_pixmap, XDefaultGCOfScreen(screen), 0, 0, 40, 40, x, y);

	lock_menu_button = (menu_button_t) { .visible = 1, .x = x, .y = y, .x_press = menu.x + x, .y_press = menu.y + y, .width = 40, .height = 40 };
}

void menu_show_sound(void) {
	int volume = sound_get_volume();
	char *text = malloc(sizeof(char) * 4);
	sprintf(text, "%d", volume);

	char *sound_logo = utils_get(UTILS_SOUND);
	Pixmap sound_pixmap = icon_get_pixmap(sound_logo, 40, 40);
	free(sound_logo);

	XDrawString(display, sound_pixmap, gc_text_black, 20, 10, text, (int) strlen(text));

	free(text);

	const int x = menu.width - 170;
	const int y = menu.height - 50;

	icon_sound = (icon_t) {
		.x = x,
		.y = y,
		.x_press = menu.x + x,
		.y_press = menu.y + y
	};
	XCopyArea(display, sound_pixmap, menu_pixmap, XDefaultGCOfScreen(screen), 0, 0, 40, 40, x, y);
}

void menu_show_brightness(void) {
	int brightness = brightness_get();
	char *text = malloc(sizeof(char) * 4);
	sprintf(text, "%d", brightness);

	char *brightness_logo = utils_get(UTILS_BRIGHTNESS);
	Pixmap brightness_pixmap = icon_get_pixmap(brightness_logo, 40, 40);
	free(brightness_logo);

	XDrawString(display, brightness_pixmap, gc_text_black, 20, 10, text, (int) strlen(text));

	free(text);

	const int x = menu.width - 220;
	const int y = menu.height - 50;

	icon_brightness = (icon_t) {
		.x = x,
		.y = y,
		.x_press = menu.x + x,
		.y_press = menu.y + y
	};
	XCopyArea(display, brightness_pixmap, menu_pixmap, XDefaultGCOfScreen(screen), 0, 0, 40, 40, x, y);
}

void menu_show_battery(void) {
	const int status = battery_get_status();
	if (status == 2) {
		return;
	}

	const int level = battery_get_level();
	char *text = malloc(sizeof(char) * 4);
	sprintf(text, "%d", level);

	int margin = 0;
	if (status == 1) {
		margin = 20;
	}

	Pixmap battery_pixmap = XCreatePixmap(display, window, 20 + margin, 40, screen_depth);

	XFillRectangle(display, battery_pixmap, gc_menu, 0, 0, 20 + margin, 40);
	XDrawRectangle(display, battery_pixmap, gc_text_black, 0, 0, 19, 39);

	const int height = (int) (38 * ((double) level) / 100);

	if (level > 20) {
		XSetForeground(display, gc_battery, color_battery_ok.pixel);
	} else {
		XSetForeground(display, gc_battery, color_battery_ko.pixel);
	}
	
	XFillRectangle(display, battery_pixmap, gc_battery, 1, 39 - height, 18, height);

	XDrawString(display, battery_pixmap, gc_text_black, 4, 12, text, (int) strlen(text));
	free(text);

	if (status == 1) {
		char *charging_logo = utils_get(UTILS_CHARGING);
		Pixmap charging_pixmap = icon_get_pixmap(charging_logo, 20, 20);
		free(charging_logo);

		XCopyArea(display, charging_pixmap, battery_pixmap, XDefaultGCOfScreen(screen), 0, 0, 20, 20, 20, 20);
	}

	const int x = menu.width - 250 - margin;
	const int y = menu.height - 50;

	XCopyArea(display, battery_pixmap, menu_pixmap, XDefaultGCOfScreen(screen), 0, 0, 20 + margin, 40, x, y);
}

int menu_buttons_on_hover(int x, int y) {
	const int new_terminal_is_hover = menu_new_terminal_is_hover(x, y);
	const int logout_is_hover = menu_logout_is_hover(x, y);
	const int lock_is_hover = menu_lock_is_hover(x, y);
	const int sound_is_hover = menu_sound_is_hover(x, y);
	const int brightness_is_hover = menu_brightness_is_hover(x, y);

	if (new_terminal_is_hover == 1) {
		XDefineCursor(display, window, hand_cursor);
		return 1;
	} else if (logout_is_hover == 1) {
		XDefineCursor(display, window, hand_cursor);
		return 2;
	} else if (lock_is_hover == 1) {
		XDefineCursor(display, window, hand_cursor);
		return 3;
	} else if (sound_is_hover == 1) {
		XDefineCursor(display, window, hand_cursor);
		return 4;
	} else if (brightness_is_hover == 1) {
		XDefineCursor(display, window, hand_cursor);
		return 5;
	}

	return 0;
}

int menu_category_buttons_on_hover(int x, int y) {
	const int left_is_hover = menu_left_category_is_hover(x, y);
	const int right_is_hover = menu_right_category_is_hover(x, y);

	if (left_is_hover == 1) {
		XDefineCursor(display, window, hand_cursor);
		return 1;
	} else if (right_is_hover == 1) {
		XDefineCursor(display, window, hand_cursor);
		return 2;
	}

	return 0;
}

void menu_category_buttons_on_press(int category_button) {
	if (category_button == 0) {
		return;
	} else if (category_button == 1) {
		menu_go_to_previous_category();
	} else if (category_button == 2) {
		menu_go_to_next_category();
	}
}

void menu_go_to_previous_category(void) {
	menu.category_index--;
	icons_init();
	menu_clear();
	menu_show();
}

void menu_go_to_next_category(void) {
	menu.category_index++;
	icons_init();
	menu_clear();
	menu_show();
}

void menu_on_scroll(int button, int x, int y) {
	const int menu_hover = menu_apps_is_hover(x, y);
	const int sound_hover = menu_sound_is_hover(x, y);
	const int brightness_hover = menu_brightness_is_hover(x, y);

	if (menu_hover == 1) {
		if (button == 4) icon_scroll_up();
		if (button == 5) icon_scroll_down();
	} else if (sound_hover == 1) {
		if (button == 4) sound_volume_up();
		if (button == 5) sound_volume_down();
	} else if (brightness_hover == 1) {
		if (button == 4) brightness_up();
		if (button == 5) brightness_down();
	}

	XEvent e = { .type = Expose };
	XSendEvent(display, window, 0, ExposureMask, &e);
}

int menu_left_category_is_hover(int x, int y) {
	return
		left_category_button.visible == 1 &&
		x >= left_category_button.x_press && x <= left_category_button.x_press + left_category_button.width &&
		y >= left_category_button.y_press && y <= left_category_button.y_press + left_category_button.height;
}

int menu_right_category_is_hover(int x, int y) {
	return
		right_category_button.visible == 1 &&
		x >= right_category_button.x_press && x <= right_category_button.x_press + right_category_button.width &&
		y >= right_category_button.y_press && y <= right_category_button.y_press + right_category_button.height;
}

int menu_apps_is_hover(int x, int y) {
	return
		x >= menu.x &&
		x <= menu.x + menu.width &&
		y >= menu.y + up_line_y &&
		y <= menu.y + bottom_line_y;
}

int menu_new_terminal_is_hover(int x, int y) {
	return
		new_terminal_menu_button.visible == 1 &&
		x >= new_terminal_menu_button.x_press && x <= new_terminal_menu_button.x_press + new_terminal_menu_button.width &&
		y >= new_terminal_menu_button.y_press && y <= new_terminal_menu_button.y_press + new_terminal_menu_button.height;
}

int menu_logout_is_hover(int x, int y) {
	return
		logout_menu_button.visible == 1 &&
		x >= logout_menu_button.x_press && x <= logout_menu_button.x_press + logout_menu_button.width &&
		y >= logout_menu_button.y_press && y <= logout_menu_button.y_press + logout_menu_button.height;
}

int menu_lock_is_hover(int x, int y) {
	return
		lock_menu_button.visible == 1 &&
		x >= lock_menu_button.x_press && x <= lock_menu_button.x_press + lock_menu_button.width &&
		y >= lock_menu_button.y_press && y <= lock_menu_button.y_press + lock_menu_button.height;
}

int menu_sound_is_hover(int x, int y) {
	return
		x >= icon_sound.x_press &&
		x <= icon_sound.x_press + 40 &&
		y >= icon_sound.y_press &&
		y <= icon_sound.y_press + 40;
}

int menu_brightness_is_hover(int x, int y) {
	return
		x >= icon_brightness.x_press &&
		x <= icon_brightness.x_press + 40 &&
		y >= icon_brightness.y_press &&
		y <= icon_brightness.y_press + 40;
}
