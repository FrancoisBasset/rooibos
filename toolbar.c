#include <X11/Xlib.h>
#include <string.h>
#include "toolbar.h"
#include "objects.h"

Font font_text_button_toolbar;
GC gc_toolbar_button;
GC gc_text_button_toolbar;

struct toolbar_button toolbar_buttons[7];

void toolbar_init(void) {
    XGCValues gcv_toolbar_button = { .foreground = white_pixel };
	gc_toolbar_button = XCreateGC(display, window, GCForeground, &gcv_toolbar_button);

    font_text_button_toolbar = XLoadFont(display, "-*-times-*-r-*-*-14-*-*-*-*-*-*-*");
	XGCValues gcv_text_button_toolbar = {
		.foreground = white_pixel,
		.font = font_text_button_toolbar
	};
	gc_text_button_toolbar = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_button_toolbar);
}

void toolbar_refresh(void) {
	XFontStruct *fontstruct = XQueryFont(display, font_text_button_toolbar);

	int button_width = screen_width / 7;
	char *labels[7] = { "New window", "Move window", "Resize window", "Minimize window", "Maximize window", "Close window", "Exit" };
	for (int i = 0; i < 7; i++) {
		int width = XTextWidth(fontstruct, labels[i], strlen(labels[i]));
		int x = (button_width - width) / 2;

		struct toolbar_button new_tbb = { labels[i], button_width * i, screen_height - 100, button_width, 50 };
		toolbar_buttons[i] = new_tbb;
		XDrawRectangle(display, window, gc_toolbar_button, button_width * i, screen_height - 100, button_width, 50);
		XDrawString(display, window, gc_text_button_toolbar, (button_width * i) + x, screen_height - 70, labels[i], strlen(labels[i]));
	}
}
