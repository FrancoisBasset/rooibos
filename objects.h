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
extern XColor color_red;
extern XColor color_green;
extern XColor color_back_menu;
extern XColor color_decorator;
extern XColor color_decorator_min;
extern XColor color_decorator_max;
extern XColor color_right_panel_hover;
extern XColor color_wallpaper_modify_button;

extern GC gc_fore_red;
extern GC gc_fore_green;
extern GC gc_text_white;
extern GC gc_text_black;
extern GC gc_category_button;
extern GC gc_icon;
extern GC gc_splash_rectangle;
extern GC gc_taskbar;
extern GC gc_taskbar_button;
extern GC gc_taskbar_button_focus;
extern GC gc_taskbar_button_hidden;
extern GC gc_taskbar_button_border;
extern GC gc_menu;
extern GC gc_battery;
extern GC gc_decorator;
extern GC gc_decorator_min;
extern GC gc_decorator_max;
extern GC gc_right_panel_hover;
extern GC gc_wallpaper_modify_button;

void objects_init(void);
XColor objects_color(unsigned short r, unsigned short g, unsigned short b);
#endif
