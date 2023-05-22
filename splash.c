#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include "objects.h"
#include "utils.h"
#include "icon.h"
#include "event.h"
#include "splash.h"
#include "wallpaper.h"

void splash_start(void) {
	wallpaper_show(1);

	int have_cache = access(utils_get(UTILS_CACHE), F_OK) == 0;

	if (have_cache == 0) {
		splash_show_rectangle();
	}

	splash_show_logo();

	if (have_cache == 0) {
		splash_show_text_at_middle("Welcome on Rooibos !", 1);
		splash_show_text_at_middle("Press Super key to show menu", 3);
		splash_show_text_at_middle("Press Super key, Esc key or click outside to hide menu", 4);
		splash_show_text_at_middle("Press Left Control key to show prompt", 6);
		splash_show_text_at_middle("Press Esc key or Left Control key to exit prompt", 7);
	}

	XFlush(display);
	icons_init();
	XDefineCursor(display, window, cursor);
	XFlush(display);

	if (have_cache == 0) {
		splash_show_text_at_middle("Press any key to continue", 9);
		
		XEvent event;
		while (1) {
			XNextEvent(display, &event);

			if (event.type == KeyPress) {
				break;
			}
		}
	} else {
		sleep(1);
	}
}

void splash_show_rectangle(void) {
	const int x = (screen_width - 600) / 2;
	const int y = (screen_height - 600) / 2;

	XFillRectangle(display, window, gc_splash_rectangle, x, y, 600, 600);
}

void splash_show_logo(void) {
	const int x = (screen_width - 300) / 2;
	const int y = (screen_height - 300) / 2;
	
	char *logo_to_use = utils_get(UTILS_LOGO_TO_USE);
	Pixmap pixmap = icon_get_pixmap(logo_to_use, 300, 300);
	free(logo_to_use);

	XCopyArea(display, pixmap, window, XDefaultGCOfScreen(screen), 0, 0, 300, 300, x, y);
}

void splash_show_text_at_middle(char *text, int index) {
	const int y = ((screen_height - 300) / 2) + (10 * index) + 320;
	const int width = XTextWidth(font_struct, text, (int) strlen(text));
	const int x = (screen_width - width) / 2;

	XDrawString(display, window, gc_text_black, x, y, text, (int) strlen(text));
}
