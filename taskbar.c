#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include "taskbar.h"
#include "objects.h"
#include "window.h"
#include "event.h"

static GC gc_taskbar;
static GC gc_taskbar_button;
static GC gc_taskbar_button_border;
static GC gc_text_button;
taskbar_t *tb;

taskbar_t* taskbar_init() {
    tb = malloc(sizeof(taskbar_t));
    tb->x = 0;
    tb->y = screen_height - 50;
    tb->width = screen_width;
    tb->height = screen_height;
    tb->tb_buttons = malloc(sizeof(taskbar_button_h));
    tb->buttons_length = 0;

    Colormap colormap_taskbar = XDefaultColormapOfScreen(screen);
    XColor color_taskbar = {
        .red = 53300,
        .green = 53300,
        .blue = 53300
    };
    XAllocColor(display, colormap_taskbar, &color_taskbar);
    XGCValues gcv_taskbar = { .foreground = color_taskbar.pixel };
    gc_taskbar = XCreateGC(display, window, GCForeground, &gcv_taskbar);

    Colormap colormap_taskbar_button = XDefaultColormapOfScreen(screen);
    XColor color_taskbar_button = {
        .red = 0,
        .green = 53300,
        .blue = 53300
    };
    XAllocColor(display, colormap_taskbar_button, &color_taskbar_button);
    XGCValues gcv_taskbar_button = { .foreground = color_taskbar_button.pixel };
    gc_taskbar_button = XCreateGC(display, window, GCForeground, &gcv_taskbar_button);

    XGCValues gcv_taskbar_button_border = {
        .foreground = color_taskbar_button.pixel,
        .background = black_pixel
    };
    gc_taskbar_button_border = XCreateGC(display, window, GCForeground | GCBackground, &gcv_taskbar_button_border);

    Font font_text_button = XLoadFont(display, "-*-times-*-r-*-*-14-*-*-*-*-*-*-*");
	XGCValues gcv_text_button = {
        .foreground = black_pixel,
        .font = font_text_button
    };
	gc_text_button = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_button);

    return tb;
}

void taskbar_update_windows() {
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

    do {
        taskbar_button_h *tb_button = malloc(sizeof(taskbar_button_h));
        tb_button->x = x;
        tb_button->y = tb->y;
        tb_button->width = 200;
        tb_button->height = 50;
        tb_button->window = w;

        tb->tb_buttons[i] = tb_button;

        i++;
        x += 200;
    } while ((w = w->next) != NULL);
}

void taskbar_refresh() {
    XFillRectangle(display, window, gc_taskbar, tb->x, tb->y, tb->width, tb->height);

    for (int i = 0; i < tb->buttons_length; i++) {
        XFillRectangle(display, window, gc_taskbar_button, tb->tb_buttons[i]->x, tb->tb_buttons[i]->y, tb->tb_buttons[i]->width, tb->tb_buttons[i]->height);
        XDrawRectangle(display, window, gc_taskbar_button_border, tb->tb_buttons[i]->x, tb->tb_buttons[i]->y, tb->tb_buttons[i]->width, tb->tb_buttons[i]->height - 1);
        XDrawString(display, window, gc_text_button, tb->tb_buttons[i]->x, tb->tb_buttons[i]->y + 28, tb->tb_buttons[i]->window->title, strlen(tb->tb_buttons[i]->window->title));
    }
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
            }
    }
}
