#ifndef OBJECTS_H
#define OBJECTS_H
#include <X11/Xlib.h>
extern Display *display;
extern Screen *screen;
extern Window root_window;
extern Visual *visual;

extern unsigned long white_pixel;
extern unsigned long black_pixel;

extern int screen_width;
extern int screen_height;
extern int screen_depth;

extern Window window;

extern Font font;
extern XFontStruct *font_struct;

extern Cursor cursor;
extern Cursor hand_cursor;
extern Cursor wait_cursor;

extern Colormap colormap;
extern XColor color_move_window;
extern XColor color_resize_window;
extern XColor color_minimize_window;
extern XColor color_maximize_window;
extern XColor color_close_window;
extern XColor color_back_menu;

extern GC gc_text_white;
extern GC gc_text_black;
extern GC gc_category_button;
extern GC gc_icon;

void objects_init(void);
#endif
