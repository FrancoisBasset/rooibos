#ifndef TASKBAR_H
#define TASKBAR_H
#include <X11/Xlib.h>
#include "window.h"

typedef struct taskbar_button_h {
    int x;
    int y;
    int width;
    int height;
    window_t *window;
} taskbar_button_h;

typedef struct taskbar_t {
    int x;
    int y;
    int width;
    int height;
    taskbar_button_h **tb_buttons;
    int buttons_length;
} taskbar_t;

extern taskbar_t *tb;

taskbar_t* taskbar_init();
void taskbar_update_windows();
void taskbar_refresh();
void taskbar_click(void);
int taskbar_is_pressed(int x, int y);
void taskbar_on_press(int x, int y);
#endif
