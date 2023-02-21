#include <stdio.h>
#include <X11/keysym.h>
#include "rooibos.h"
#include "objects.h"
#ifdef WILLDEBUG
#include "debug.h"
#endif
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
#ifdef WILLDEBUG
	debug_init();
#endif
	XStoreName(display, window, "rooibos");
	XSelectInput(display, root_window, SubstructureNotifyMask);
	XMapWindow(display, window);
	XFlush(display);

	XDefineCursor(display, window, wait_cursor);
	
	toolbar_init();
	taskbar_init();
	prompt_init();
	windows_init();
	menu_init();

	splash_start();

	event_on_expose();
	
	while (1) {
		int quit = handle_event();

		if (quit == 1) {
			break;
		}
	}

#ifdef WILLDEBUG
	if (debug) {
		windows_print(debug);
		debug_free();
	}
#endif

	XCloseDisplay(display);
}
