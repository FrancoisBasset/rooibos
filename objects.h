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
extern int screen_depth;
extern Font font;
extern XFontStruct *font_struct;
extern Cursor cursor;
extern Cursor hand_cursor;
extern Cursor wait_cursor;
extern GC gc_text_black;
extern GC gc_text_white;
extern Colormap colormap;

void objects_init(void);
#endif
