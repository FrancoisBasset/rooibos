#ifndef TASKBAR_H
#define TASKBAR_H
#include <X11/Xlib.h>
#include "window.h"

struct taskbar_button_h {
    int x;
    int y;
    int width;
    int height;
    struct window_t *window;
};

struct taskbar_t {
    int x;
    int y;
    int width;
    int height;
    struct taskbar_button_h **tb_buttons;
    int buttons_length;
};

extern struct taskbar_t *tb;

struct taskbar_t* taskbar_init();
void taskbar_update_windows();
void taskbar_refresh();
void taskbar_click(void);
int taskbar_is_pressed(int x, int y);
void taskbar_on_press(int x, int y);
#endif
