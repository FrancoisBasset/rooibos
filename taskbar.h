#ifndef TASKBAR_H
#define TASKBAR_H
#include <X11/Xlib.h>
#include "window.h"

struct taskbar_button {
    int x;
    int y;
    int width;
    int height;
    struct window *window;
};

struct taskbar {
    int x;
    int y;
    int width;
    int height;
    struct taskbar_button **tb_buttons;
    int buttons_length;
};

struct taskbar* taskbar_init(Display *display, Window window, Screen *screen, int width, int height);
void taskbar_set_windows(struct taskbar *tb, struct windows *ws);
void taskbar_refresh(Display *display, Window window, struct taskbar *tb);
void taskbar_click(void);
#endif