#include <string.h>
#include <X11/Xlib.h>
#include "toolbar.h"
#include "objects.h"
#include "window.h"
#include "event.h"

toolbar_button_t toolbar_buttons[1];

void toolbar_init(void) {

}

void toolbar_show(void) {
	int button_width = screen_width;

	XColor colors[1] = { color_resize_window };
	char *labels[1] = { "Resize window" };

	for (int i = 0; i < 1; i++) {
		int width = XTextWidth(font_struct, labels[i], (int) strlen(labels[i]));
		int x = (button_width - width) / 2;

		toolbar_button_t new_tbb = { labels[i], button_width * i, screen_height - 100, button_width, 50 };
		toolbar_buttons[i] = new_tbb;

		XSetForeground(display, gc_toolbar_button, colors[i].pixel);

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
	int position = (int) (((double) x / (double) screen_width) * 1);

	switch (position) {
		case 0:
			mode = 'r';
			break;
		default:
			return 0;
	}

	return 0;
}
