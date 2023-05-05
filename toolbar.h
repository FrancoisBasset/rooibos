#ifndef TOOLBAR_H
#define TOOLBAR_H
#include <X11/Xlib.h>
typedef struct toolbar_button_t {
    char *title;
    int x;
    int y;
    int width;
    int height;
} toolbar_button_t;

void toolbar_init(void);
void toolbar_show(void);
void toolbar_hide(void);
int toolbar_is_hover(int y);
int toolbar_on_press(int x);
#endif
