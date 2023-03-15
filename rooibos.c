#include <stdio.h>
#include <X11/keysym.h>
#include "rooibos.h"
#include "objects.h"
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
	windows_print();
#endif

	XCloseDisplay(display);
}
