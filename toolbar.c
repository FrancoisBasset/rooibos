#include <string.h>
#include <X11/Xlib.h>
#include "toolbar.h"
#include "objects.h"
#include "window.h"
#include "event.h"

XGCValues gcv_toolbar_button;
GC gc_toolbar_button;

toolbar_button_t toolbar_buttons[5];

void toolbar_init(void) {
    gcv_toolbar_button = (XGCValues) { .foreground = white_pixel };
	gc_toolbar_button = XCreateGC(display, window, GCForeground, &gcv_toolbar_button);
}

void toolbar_show(void) {
	int button_width = screen_width / 5;

	XColor colors[5] = { color_move_window, color_resize_window, color_minimize_window, color_maximize_window, color_close_window };
	char *labels[5] = { "Move window", "Resize window", "Minimize window", "Maximize window", "Close window" };

	for (int i = 0; i < 5; i++) {
		int width = XTextWidth(font_struct, labels[i], (int) strlen(labels[i]));
		int x = (button_width - width) / 2;

		toolbar_button_t new_tbb = { labels[i], button_width * i, screen_height - 100, button_width, 50 };
		toolbar_buttons[i] = new_tbb;

		gcv_toolbar_button.foreground = colors[i].pixel;
		XChangeGC(display, gc_toolbar_button, GCForeground, &gcv_toolbar_button);

		XFillRectangle(display, window, gc_toolbar_button, button_width * i, screen_height - 100, button_width, 50);
		XDrawString(display, window, gc_text_white, (button_width * i) + x, screen_height - 70, labels[i], (int) strlen(labels[i]));
	}
}

void toolbar_hide(void) {
	XClearArea(display, window, 0, screen_height - 100, screen_width, 100, 1);
}

int toolbar_is_hover(int y) {
	return y >= screen_height - 100 && y <= screen_height - 50;
}

int toolbar_on_press(int x) {
	int position = (int) (((double) x / (double) screen_width) * 5);

	switch (position) {
		case 0:
			mode = 'm';
			break;
		case 1:
			mode = 'r';
			break;
		case 2:
			if (window_focus != NULL) {
				XUnmapWindow(display, window_focus->id);
				window_focus->visible = 0;
			}
			break;
		case 3:
			if (window_focus != NULL) {
				XMoveResizeWindow(display, window_focus->id, 0, 0, screen_width, screen_height - 100);
			}
			break;
		case 4:
			if (window_focus != NULL) {
				XDestroyWindow(display, window_focus->id);
				window_focus = NULL;
			}
			break;
		default:
			return 0;
	}

	return 0;
}
