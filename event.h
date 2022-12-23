#ifndef EVENT_H
#define EVENT_H
#include <X11/Xlib.h>
int handle_event(void);
void on_expose(void);
int on_button_press(int x, int y);
void on_motion(int x, int y);
void on_configure(Window child, int x, int y, int width, int height);
void on_destroy(Window child);
void new_window(void);
#endif
