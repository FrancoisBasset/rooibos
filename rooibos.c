#include <stdio.h>
#include "rooibos.h"
#include "objects.h"
#include "toolbar.h"
#include "taskbar.h"
#include "event.h"
#include "icon.h"
#include "menu.h"

FILE *debug;

void rooibos(void) {
	objects_init();
	
	XStoreName(display, window, "rooibos");
	XSelectInput(display, root_window, SubstructureNotifyMask);
	XMapWindow(display, window);
	XFlush(display);

	debug = fopen("./debug.txt", "w");

	XDefineCursor(display, window, wait_cursor);
	RsvgHandle *rooibos_icon = icon_get_surface_svg("./assets/logo.svg");
	int x = (screen_width - 300) / 2;
    int y = (screen_height - 300) / 2;
	icon_draw_svg(rooibos_icon, "", x, y, 300, 300);

	int width = XTextWidth(font_struct, "Welcome on Rooibos !", 20);
	x = (screen_width - width) / 2;
	XDrawString(display, window, gc_text_white, x, y + 350, "Welcome on Rooibos !", 20);

	width = XTextWidth(font_struct, "Press Meta key to show menu", 27);
	x = (screen_width - width) / 2;
	XDrawString(display, window, gc_text_white, x, y + 370, "Press Meta key to show menu", 27);

	XFlush(display);
	icons_init();
    XDefineCursor(display, window, cursor);
    XFlush(display);

	toolbar_init();
	taskbar_init();
	windows_init();
	menu_init();
	
	while (1) {
		int quit = handle_event();

		if (quit == 1) {
			break;
		}
	}

	if (debug) {
		windows_print(debug);
		fclose(debug);
	}

	XCloseDisplay(display);
}
