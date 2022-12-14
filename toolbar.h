#ifndef TOOLBAR_H
#define TOOLBAR_H
#include <X11/Xlib.h>
struct toolbar_button {
    char *title;
    int x;
    int y;
    int width;
    int height;
};

extern Font font_text_button_toolbar;
extern GC gc_toolbar_button;
extern GC gc_text_button_toolbar;

void toolbar_init(void);
void toolbar_refresh(void);
int toolbar_is_pressed(int y);
int toolbar_on_click(int x);
#endif
