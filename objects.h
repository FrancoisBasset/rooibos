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
extern Cursor resize_cursor;

extern Colormap colormap;
extern XColor color_back_menu;
extern XColor color_battery_ok;
extern XColor color_battery_ko;
extern XColor color_bar;

extern GC gc_text_white;
extern GC gc_text_black;
extern GC gc_category_button;
extern GC gc_icon;
extern GC gc_bar;
extern GC gc_splash_rectangle;
extern GC gc_taskbar;
extern GC gc_taskbar_button;
extern GC gc_taskbar_button_focus;
extern GC gc_taskbar_button_hidden;
extern GC gc_taskbar_button_border;
extern GC gc_menu;
extern GC gc_battery;

void objects_init(void);
XColor objects_color(unsigned short r, unsigned short g, unsigned short b);
#endif
