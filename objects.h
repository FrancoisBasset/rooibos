#ifndef OBJECTS_H
#define OBJECTS_H
#include <X11/Xlib.h>
extern Display *display;
extern Screen *screen;
extern Window root_window;
extern Window window;
extern unsigned long white_pixel;
extern unsigned long black_pixel;
extern int screen_width;
extern int screen_height;

void objects_init(void);
#endif
