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

extern struct taskbar *tb;

struct taskbar* taskbar_init();
void taskbar_update_windows();
void taskbar_refresh();
void taskbar_click(void);
#endif
