#include <string.h>
#include <X11/Xlib.h>
#include "decorator.h"
#include "objects.h"
#include "window.h"
#include "event.h"
#include "taskbar.h"
#include "wallpaper.h"

window_t *current;
int pressed = 0;

void decorator_show_all() {
	Pixmap decorator_pixmap;

	window_reset();

	window_t *w = window_next();
	while (w != NULL) {
		if (w->visible == 0) {
			w = window_next();
			continue;
		}

		decorator_pixmap = XCreatePixmap(display, window, w->width, 20, screen_depth);
		decorator_t d = w->decorator;

		XFillRectangle(display, decorator_pixmap, gc_decorator, 0, 0, w->width, 20);
		XDrawString(display, decorator_pixmap, gc_text_white, 7, 13, w->title, (int) strlen(w->title));
		
		XFillArc(display, decorator_pixmap, gc_decorator_min, d.min_start, 0, 20, 20, 360 * 64, 360 * 64);
		XDrawArc(display, decorator_pixmap, gc_text_white, d.min_start, 0, 20, 19, 360 * 64, 360 * 64);
		XFillArc(display, decorator_pixmap, gc_decorator_max, d.max_start, 0, 20, 20, 360 * 64, 360 * 64);
		XDrawArc(display, decorator_pixmap, gc_text_white, d.max_start, 0, 20, 19, 360 * 64, 360 * 64);
		XFillArc(display, decorator_pixmap, gc_fore_red, d.close_start, 0, 20, 20, 360 * 64, 360 * 64);
		XDrawArc(display, decorator_pixmap, gc_text_white, d.close_start, 0, 20, 19, 360 * 64, 360 * 64);

		XCopyArea(display, decorator_pixmap, window, XDefaultGCOfScreen(screen), 0, 0, w->width, 20, w->x + 1, w->y - 20);

		w = window_next();
	}
}

int decorator_on_hover(int x, int y) {
	window_reset();

	window_t *w = window_next();
	while (w != NULL) {
		if (w->visible == 1 && w->decorator.y <= y && y <= w->decorator.y + 20) {
			if (x >= w->decorator.x && x <= w->decorator.x + w->decorator.min_start) {
				if (current != NULL && pressed == 1) {
					XMoveWindow(display, current->id, x - 40, y + 10);

					if (current->id == wallpaper_window) {
						current->x = x - 40;
						current->y = y + 10;
					}
				}
				
				current = w;
				return 1;
			}

			if (x >= w->decorator.x + w->decorator.min_start && x <= w->decorator.x + w->decorator.min_end) {
				current = w;
				return 2;
			}

			if (x >= w->decorator.x + w->decorator.max_start&& x <= w->decorator.x + w->decorator.max_end) {
				current = w;
				return 3;
			}

			if (x >= w->decorator.x + w->decorator.close_start && x <= w->decorator.x + w->decorator.close_end) {
				current = w;
				return 4;
			}
		}

		if (x >= w->x + w->width &&
			x <= w->x + w->width + 20 &&
			y >= w->y + w->height &&
			y <= w->y + w->height + 20) {

			if (current != NULL && pressed == 1) {
				XResizeWindow(display, current->id, x - current->x - 5, y - current->y - 5);

				if (current->id == wallpaper_window) {
					current->width = x - current->x - 5;
					current->height = y - current->y - 5;
				}

				XEvent e = { .type = Expose };
				XSendEvent(display, window, 0, ExposureMask, &e);
			}

			current = w;
			return 5;
		}
		
		w = window_next();
	}

	return 0;
}

Window decorator_get_selected(void) {
	return current->id;
}

void decorator_set_selected_null() {
	current = NULL;
	pressed = 0;
}

void decorator_on_press(int x, int y) {
	int button = decorator_on_hover(x, y);

	Window window_selected = decorator_get_selected();
	pressed = 1;

	switch (button) {
		case 1:
		case 5:
			window_focus = window_get(window_selected);
			break;
		case 2:
			XUnmapWindow(display, window_focus->id);
			if (window_focus->id == window_selected) {
				window_focus->visible = 0;
				taskbar_update_windows();
			}
			break;
		case 3:
			XMoveResizeWindow(display, window_selected, 0, 20, screen_width, screen_height - 70);
			window_focus->x = 0;
			window_focus->y = 20;
			window_focus->width = screen_width;
			XEvent e = { .type = Expose };
			XSendEvent(display, window, 0, ExposureMask, &e);
			break;
		case 4:
			XDestroyWindow(display, window_selected);

			if (window_selected == wallpaper_window) {
				window_delete(wallpaper_window);
				taskbar_update_windows();
				wallpaper_window = -1;
			}
			
			if (window_focus->id == window_selected) {
				window_focus = NULL;
			}
			break;
	}
}
