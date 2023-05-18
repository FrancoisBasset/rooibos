#include <stdio.h>
#include <X11/keysym.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
#include "rooibos.h"
#include "objects.h"
#include "taskbar.h"
#include "event.h"
#include "icon.h"
#include "menu.h"
#include "prompt.h"
#include "utils.h"
#include "splash.h"
#include "sound.h"

void rooibos(void) {
	objects_init();

	XStoreName(display, window, "rooibos");
	XSelectInput(display, root_window, SubstructureNotifyMask);
	XMapWindow(display, window);
	XFlush(display);

	XDefineCursor(display, window, wait_cursor);
	
	taskbar_init();
	prompt_init();
	windows_init();
	menu_init();
	sound_init();

	gdk_pixbuf_xlib_init(display, XDefaultScreen(display));

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

	windows_free();

	XCloseDisplay(display);
}
