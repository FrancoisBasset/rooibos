#include <stdio.h>
#include "rooibos.h"
#include "objects.h"
#include "toolbar.h"
#include "taskbar.h"
#include "event.h"
#include "icon.h"

FILE *debug;

void rooibos(void) {
	objects_init();
	
	XStoreName(display, window, "rooibos");
	XSelectInput(display, root_window, SubstructureNotifyMask);
	XMapWindow(display, window);
	XFlush(display);

	debug = fopen("./debug.txt", "w");

	toolbar_init();
	taskbar_init();
	windows_init();
	icons_init();

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
