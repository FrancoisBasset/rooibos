#include <X11/Xlib.h>
#include <librsvg-2.0/librsvg/rsvg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objects.h"
#include "utils.h"
#include "icon.h"
#include "event.h"
#include "splash.h"

void splash_start(void) {
	show_wallpaper();

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
		
		int quit = 0;
		while (1) {
			XEvent event;
    		XNextEvent(display, &event);

			switch (event.type) {
				case KeyPress:
					quit = 1;
					break;
			}

			if (quit == 1) {
				break;
			}
		}
	} else {
		sleep(1);
	}
}

void splash_show_rectangle(void) {
	int x = (screen_width - 600) / 2;
    int y = (screen_height - 600) / 2;

	cairo_surface_t *x11_surface = cairo_xlib_surface_create(display, window, XDefaultVisualOfScreen(screen), screen_width, screen_height);
    cairo_t *context = cairo_create(x11_surface);

    cairo_set_source_rgba(context, 0.85, 0.85, 0.85, 0.8);

    cairo_move_to(context, x, y);

    cairo_line_to(context, x + 600, y);
    cairo_line_to(context, x + 600, y + 600);
    cairo_line_to(context, x, y + 600);
    cairo_line_to(context, x, y);

    cairo_stroke_preserve(context);
    cairo_fill(context);
}

void splash_show_logo(void) {
	char *logo_file_path = malloc(sizeof(char) * 200);
	strcpy(logo_file_path, "");
	if (access("/usr/share/rooibos/logo.svg", F_OK) == 0) {
		strcpy(logo_file_path, "/usr/share/rooibos/logo.svg");
	}

	char *logo_path = utils_get(UTILS_LOGO);
    if (access(logo_path, F_OK) == 0) {
        strcpy(logo_file_path, logo_path);
	}
    free(logo_path);
	
	if (access("./assets/logo.svg", F_OK) == 0) {
		strcpy(logo_file_path, "./assets/logo.svg");
	}

	if (strcmp(logo_file_path, "") != 0) {
		RsvgHandle *rooibos_icon = icon_get_surface_svg(logo_file_path);
		int x = (screen_width - 300) / 2;
    	int y = (screen_height - 300) / 2;
		icon_draw_svg(rooibos_icon, "", x, y, 300, 300);
	}
	free(logo_file_path);
}

void splash_show_text_at_middle(char *text, int index) {
	const int y = ((screen_height - 300) / 2) + (10 * index) + 320;
	const int width = XTextWidth(font_struct, text, (int) strlen(text));
	const int x = (screen_width - width) / 2;

	XDrawString(display, window, gc_text_black, x, y, text, (int) strlen(text));
}
