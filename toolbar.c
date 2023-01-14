#include <X11/Xlib.h>
#include <string.h>
#include "toolbar.h"
#include "objects.h"
#include "window.h"
#include "event.h"

GC gc_toolbar_button;

toolbar_button_t toolbar_buttons[7];

void toolbar_init(void) {
    XGCValues gcv_toolbar_button = { .foreground = white_pixel };
	gc_toolbar_button = XCreateGC(display, window, GCForeground, &gcv_toolbar_button);
}

void toolbar_refresh(void) {
	int button_width = screen_width / 7;
	char *labels[7] = { "New window", "Move window", "Resize window", "Minimize window", "Maximize window", "Close window", "Exit" };
	for (int i = 0; i < 7; i++) {
		int width = XTextWidth(font_struct, labels[i], (int) strlen(labels[i]));
		int x = (button_width - width) / 2;

		toolbar_button_t new_tbb = { labels[i], button_width * i, screen_height - 100, button_width, 50 };
		toolbar_buttons[i] = new_tbb;
		XDrawRectangle(display, window, gc_toolbar_button, button_width * i, screen_height - 100, button_width, 50);
		XDrawString(display, window, gc_text_white, (button_width * i) + x, screen_height - 70, labels[i], (int) strlen(labels[i]));
	}
}

int toolbar_is_pressed(int y) {
	return y >= screen_height - 100 && y <= screen_height - 50;
}

int toolbar_on_click(int x) {
	int position = (int) (((double) x / (double) screen_width) * 7);

	switch (position) {
		case 0:
			new_window();
			break;
		case 1:
			mode = 'm';
			break;
		case 2:
			mode = 'r';
			break;
		case 3:
			if (window_focus != NULL) {
				XUnmapWindow(display, window_focus->id);
				window_focus->visible = 0;
			}
			break;
		case 4:
			if (window_focus != NULL) {
				XMoveResizeWindow(display, window_focus->id, 0, 0, screen_width, screen_height - 100);
			}
			break;
		case 5:
			if (window_focus != NULL) {
				XDestroyWindow(display, window_focus->id);
				window_focus = NULL;
			}
			break;
		case 6:
			return 1;
		default:
			return 0;
	}

	return 0;
}
