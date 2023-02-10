#include <stdio.h>
#include <X11/keysym.h>
#include "rooibos.h"
#include "objects.h"
#include "debug.h"
#include "toolbar.h"
#include "taskbar.h"
#include "event.h"
#include "icon.h"
#include "menu.h"
#include "prompt.h"
#include "utils.h"
#include "splash.h"

void rooibos(void) {
	objects_init();
	debug_init();
	
	XStoreName(display, window, "rooibos");
	XSelectInput(display, root_window, SubstructureNotifyMask);
	XMapWindow(display, window);
	XFlush(display);

	XDefineCursor(display, window, wait_cursor);

	splash_start();

	toolbar_init();
	taskbar_init();
	prompt_init();
	windows_init();
	menu_init();

	event_on_expose();
	
	while (1) {
		int quit = handle_event();

		if (quit == 1) {
			break;
		}
	}

	if (debug) {
		windows_print(debug);
		debug_free();
	}

	XCloseDisplay(display);
}
