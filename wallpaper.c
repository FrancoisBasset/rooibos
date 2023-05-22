#include <stdlib.h>
#include <string.h>
#include <X11/keysym.h>
#include <unistd.h>
#include "wallpaper.h"
#include "objects.h"
#include "window.h"
#include "taskbar.h"
#include "cache.h"
#include "utils.h"
#include "menu.h"
#include "icon.h"

Pixmap wallpaper_surface = -1;
Pixmap thumbnail = -1;
Window wallpaper_window = -1;
int editing = 0;
char *wallpaper_input = NULL;

char* wallpaper_get_current(void) {
	char *wallpaper_file = cache_get_wallpaper();

	if (wallpaper_file == NULL) {
		wallpaper_file = utils_get(UTILS_WALLPAPER_TO_USE);
	}

	return wallpaper_file;
}

void wallpaper_show(int splash) {
	int height = screen_height - 50;
	if (menu.is_showed == 1 || splash == 1) {
		height = screen_height;
	}

	if (wallpaper_surface == -1) {
		char *wallpaper_file = wallpaper_get_current();
		
		wallpaper_surface = icon_get_pixmap(wallpaper_file, screen_width, screen_height);
		free(wallpaper_file);
	}

	XCopyArea(display, wallpaper_surface, window, XDefaultGCOfScreen(screen), 0, 0, screen_width, height, 0, 0);
}

void wallpaper_open_window(void) {
	int width = 800;
	int height = 600;
	int x = (screen_width - width) / 2;
	int y = (screen_height - height) / 2;

	XSetWindowAttributes xswa_wallpaper = {
		.background_pixel = white_pixel,
		.event_mask = ButtonPressMask | KeyPressMask
	};
	wallpaper_window = XCreateWindow(display, window, x, y, width, height, 0, screen_depth, InputOutput, visual, CWBackPixel | CWEventMask, &xswa_wallpaper);
	XMapWindow(display, wallpaper_window);

	wallpaper_input = wallpaper_get_current();
	wallpaper_expose();
	
	window_t *new_window = window_init(wallpaper_window, "Change wallpaper", x, y, width, height);
	window_add(new_window);

	taskbar_update_windows();
}

void wallpaper_expose(void) {
	if (wallpaper_window == -1) {
		return;
	}

	XClearArea(display, wallpaper_window, 200, 500, 400, 20, 1);
	XDrawRectangle(display, wallpaper_window, gc_text_black, 200, 500, 400, 20);
	XDrawString(display, wallpaper_window, gc_text_black, 205, 515, wallpaper_input, (int) strlen(wallpaper_input));

	XFillRectangle(display, wallpaper_window, gc_wallpaper_modify_button, 300, 550, 200, 20);

	XDrawString(display, wallpaper_window, gc_text_black, 350, 564, "Change wallpaper", (int) strlen("Change wallpaper"));

	wallpaper_update_thumbnail();

	if (editing == 1) {
		XDrawRectangle(display, wallpaper_window, gc_fore_red, 200, 500, 400, 20);
	}
}

void wallpaper_on_close(void) {
	wallpaper_window = -1;
}

void wallpaper_on_press(int x, int y) {
	editing = 0;

	if (x >= 200 && x <= 600 && y >= 500 && y <= 520) {
		editing = 1;
	} else if (x >= 300 && x <= 500 && y >= 550 && y <= 570) {
		cache_set_wallpaper(wallpaper_input);
		wallpaper_surface = -1;
	}

	XEvent e = { .type = Expose };
	XSendEvent(display, window, 0, ExposureMask, &e);
}

void wallpaper_on_key_press(XKeyEvent key_event) {
	if (editing == 0) {
		return;
	}

	KeySym key_sym = XLookupKeysym(&key_event, 0);

	XIM xim = XOpenIM(display, NULL, NULL, NULL);
	XIC xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, wallpaper_window, NULL);
	char *typed = malloc(sizeof(char) * 10);
	strcpy(typed, "");

	switch (key_sym) {
		case XK_BackSpace: {
			if (strlen(wallpaper_input) > 0) {
				wallpaper_input[(int) strlen(wallpaper_input) - 1] = '\0';
			}
			break;
		}
		default:
			XmbLookupString(xic, &key_event, typed, sizeof(typed), NULL, NULL);
			if (strlen(typed) == 1) {
				strcat(wallpaper_input, typed);
			}
			break;
	}

	thumbnail = -1;
	free(typed);

	XEvent e = { .type = Expose };
	XSendEvent(display, window, 0, ExposureMask, &e);
}

void wallpaper_update_thumbnail(void) {
	if (access(wallpaper_input, F_OK) == 0 && (strstr(wallpaper_input, ".png") || strstr(wallpaper_input, ".jpg"))) {
		if (thumbnail == -1) {
			thumbnail = icon_get_pixmap(wallpaper_input, 700, 394);
		}
		XCopyArea(display, thumbnail, wallpaper_window, XDefaultGCOfScreen(screen), 0, 0, 700, 394, 50, 50);
	} else {
		XClearArea(display, wallpaper_window, 50, 50, 700, 394, 1);
	}
}
