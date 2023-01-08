#ifndef EVENT_H
#define EVENT_H
#include <X11/Xlib.h>

extern char mode;
extern struct window_t *window_focus;
extern int moving;
extern int resizing;

int handle_event(void);
void event_on_expose(void);
int event_on_button_press(int x, int y);
void event_on_motion(int x, int y);
void event_on_configure(Window child, int x, int y, int width, int height);
void event_on_property(Window child);
void event_on_destroy(Window child);
void new_window(void);
#endif
