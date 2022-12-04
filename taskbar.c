#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include "taskbar.h"

static GC gc_taskbar;
static GC gc_taskbar_button;
static GC gc_taskbar_button_border;
static GC gc_text_button;

struct taskbar* taskbar_init(Display *display, Window window, Screen *screen, int width, int height) {
    struct taskbar *tb = malloc(sizeof(struct taskbar));
    tb->x = 0;
    tb->y = height - 50;
    tb->width = width;
    tb->height = height;
    tb->tb_buttons = malloc(sizeof(struct taskbar_button));
    tb->buttons_length = 0;

    Colormap colormap_taskbar = XDefaultColormapOfScreen(screen);
    XColor color_taskbar;
    color_taskbar.red = 53300;
    color_taskbar.green = 53300;
    color_taskbar.blue = 53300;
    XAllocColor(display, colormap_taskbar, &color_taskbar);
    XGCValues gcv_taskbar;
    gcv_taskbar.foreground = color_taskbar.pixel;
    gc_taskbar = XCreateGC(display, window, GCForeground, &gcv_taskbar);

    Colormap colormap_taskbar_button = XDefaultColormapOfScreen(screen);
    XColor color_taskbar_button;
    color_taskbar_button.red = 0;
    color_taskbar_button.green = 53300;
    color_taskbar_button.blue = 53300;
    XAllocColor(display, colormap_taskbar, &color_taskbar_button);
    XGCValues gcv_taskbar_button;
    gcv_taskbar_button.foreground = color_taskbar_button.pixel;
    gc_taskbar_button = XCreateGC(display, window, GCForeground, &gcv_taskbar_button);

    XGCValues gcv_taskbar_button_border;
    gcv_taskbar_button.foreground = color_taskbar_button.pixel;
    gcv_taskbar_button.background = XBlackPixelOfScreen(screen);
    gc_taskbar_button_border = XCreateGC(display, window, GCForeground | GCBackground, &gcv_taskbar_button_border);

    Font font_text_button = XLoadFont(display, "-*-times-*-r-*-*-14-*-*-*-*-*-*-*");
	XGCValues gcv_text_button;
	gcv_text_button.foreground = XBlackPixelOfScreen(screen);
	gcv_text_button.font = font_text_button;
	gc_text_button = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_button);

    return tb;
}

void taskbar_set_windows(struct taskbar *tb, struct windows *ws) {
    for (int i = 0; i < tb->buttons_length; i++) {
        free(tb->tb_buttons[i]);
    }
    free(tb->tb_buttons);

    tb->buttons_length = ws->length;
    tb->tb_buttons = malloc(sizeof(struct taskbar_button) * ws->length);

    if (ws->first == NULL) {
        return;
    }

    struct window *w = ws->first;

    int i = 0;
    int x = 0;

    do {
        struct taskbar_button *tb_button = malloc(sizeof(struct taskbar_button));
        tb_button->x = x;
        tb_button->y = tb->y;
        tb_button->width = 100;
        tb_button->height = 50;
        tb_button->window = w;

        tb->tb_buttons[i] = tb_button;

        i++;
        x += 100;
    } while ((w = w->next) != NULL);
}

void taskbar_refresh(Display *display, Window window, struct taskbar *tb) {
    XFillRectangle(display, window, gc_taskbar, tb->x, tb->y, tb->width, tb->height);

    for (int i = 0; i < tb->buttons_length; i++) {
        XFillRectangle(display, window, gc_taskbar_button, tb->tb_buttons[i]->x, tb->tb_buttons[i]->y, tb->tb_buttons[i]->width, tb->tb_buttons[i]->height);
        XDrawRectangle(display, window, gc_taskbar_button_border, tb->tb_buttons[i]->x, tb->tb_buttons[i]->y, tb->tb_buttons[i]->width, tb->tb_buttons[i]->height - 1);
        XDrawString(display, window, gc_text_button, tb->tb_buttons[i]->x, tb->tb_buttons[i]->y + 28, tb->tb_buttons[i]->window->title, strlen(tb->tb_buttons[i]->window->title));
    }
}