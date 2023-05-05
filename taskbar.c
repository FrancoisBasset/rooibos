#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include "taskbar.h"
#include "objects.h"
#include "window.h"
#include "event.h"

taskbar_t *tb;

static Pixmap pixmap;
int refresh_pixmap = 1;

taskbar_t* taskbar_init() {
    tb = malloc(sizeof(taskbar_t));
    tb->x = 0;
    tb->y = screen_height - 50;
    tb->width = screen_width;
    tb->height = screen_height;
    tb->tb_buttons = malloc(sizeof(taskbar_button_h));
    tb->buttons_length = 0;

	pixmap = XCreatePixmap(display, window, tb->width, tb->height, XDefaultDepthOfScreen(screen));

    return tb;
}

void taskbar_update_windows() {
	refresh_pixmap = 1;

    windows_t *ws = windows_get();

    for (int i = 0; i < tb->buttons_length; i++) {
        free(tb->tb_buttons[i]);
    }
    free(tb->tb_buttons);

    tb->buttons_length = ws->length;
    tb->tb_buttons = malloc(sizeof(taskbar_button_h) * ws->length);

    if (ws->first == NULL) {
        return;
    }

    window_t *w = ws->first;

    int i = 0;
    int x = 0;

    int width = 200;
    if (width * ws->length > screen_width) {
        width = screen_width / ws->length;
    }

    do {
        taskbar_button_h *tb_button = malloc(sizeof(taskbar_button_h));
        tb_button->x = x;
        tb_button->y = tb->y;
        tb_button->width = width;
        tb_button->height = 50;
        tb_button->window = w;

        tb->tb_buttons[i] = tb_button;

        i++;
        x += width + 1;

		if (window_focus == NULL) {
			window_focus = w;
		}
    } while ((w = w->next) != NULL);
}

void taskbar_show() {
	if (refresh_pixmap == 1) {
		XFillRectangle(display, pixmap, gc_taskbar, 0, 0, tb->width, tb->height);

		for (int i = 0; i < tb->buttons_length; i++) {
			if (tb->tb_buttons[i]->window->visible == 1) {
				if (window_focus == tb->tb_buttons[i]->window) {
					XFillRectangle(display, pixmap, gc_taskbar_button_focus, tb->tb_buttons[i]->x, 0, tb->tb_buttons[i]->width, tb->tb_buttons[i]->height);
				} else {
					XFillRectangle(display, pixmap, gc_taskbar_button, tb->tb_buttons[i]->x, 0, tb->tb_buttons[i]->width, tb->tb_buttons[i]->height);
				}
			} else {
				XFillRectangle(display, pixmap, gc_taskbar_button_hidden, tb->tb_buttons[i]->x, 0, tb->tb_buttons[i]->width, tb->tb_buttons[i]->height);
			}
			XDrawRectangle(display, pixmap, gc_taskbar_button_border, tb->tb_buttons[i]->x, 0, tb->tb_buttons[i]->width, tb->tb_buttons[i]->height - 1);
			XDrawString(display, pixmap, gc_text_white, tb->tb_buttons[i]->x, 28, tb->tb_buttons[i]->window->title, (int) strlen(tb->tb_buttons[i]->window->title));
		}
		refresh_pixmap = 0;
	}

	XCopyArea(display, pixmap, window, XDefaultGCOfScreen(screen), 0, 0, tb->width, tb->height, tb->x, tb->y);
}

void taskbar_hide(void) {
	XClearArea(display, window, 0, screen_height, screen_width, 100, 1);
}

int taskbar_is_pressed(int x, int y) {
    return x >= tb->x && x <= screen_width && y >= tb->y && y <= screen_height;
}

void taskbar_on_press(int x, int y) {
    for (int i = 0; i < tb->buttons_length; i++) {
        taskbar_button_h *button = tb->tb_buttons[i];

        if (x >= button->x && x <= button->x + button->width &&
            y >= button->y && y <= button->y + button->height) {
                if (button->window->visible == 1) {
                    window_focus = NULL;
                    XUnmapWindow(display, button->window->id);
                    button->window->visible = 0;
                } else {
                    window_focus = button->window;
                    XMapWindow(display, button->window->id);
                    button->window->visible = 1;
                }
				refresh_pixmap = 1;
            }
    }

    taskbar_show();
}
