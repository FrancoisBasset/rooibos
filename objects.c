#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include "objects.h"

Display *display;
Screen *screen;
Window root_window;
Visual *visual;

unsigned long white_pixel;
unsigned long black_pixel;

int screen_width;
int screen_height;
int screen_depth;

Window window;

Font font;
XFontStruct *font_struct;

Cursor cursor;
Cursor hand_cursor;
Cursor wait_cursor;

Colormap colormap;
XColor color_move_window;
XColor color_resize_window;
XColor color_minimize_window;
XColor color_maximize_window;
XColor color_close_window;
XColor color_category_button;
XColor color_back_menu;
XColor color_battery_ok;
XColor color_battery_ko;
XColor color_bar;

GC gc_text_white;
GC gc_text_black;
GC gc_category_button;
GC gc_icon;
GC gc_bar;
GC gc_splash_rectangle;
GC gc_taskbar;
GC gc_taskbar_button;
GC gc_taskbar_button_focus;
GC gc_taskbar_button_hidden;
GC gc_taskbar_button_border;
GC gc_toolbar_button;
GC gc_menu;
GC gc_battery;

void objects_init(void) {
    display = XOpenDisplay(NULL);
    screen = XDefaultScreenOfDisplay(display);
    root_window = XDefaultRootWindow(display);
    visual = XDefaultVisualOfScreen(screen);

    white_pixel = XWhitePixelOfScreen(screen);
    black_pixel = XBlackPixelOfScreen(screen);

    screen_width = XWidthOfScreen(screen);
    screen_height = XHeightOfScreen(screen);
    screen_depth = XDefaultDepthOfScreen(screen);

    XSetWindowAttributes window_attributes = {
        .cursor = cursor,
        .event_mask = ExposureMask | ButtonPressMask | PointerMotionMask | KeyPressMask | FocusChangeMask
    };
    window = XCreateWindow(display, root_window, 0, 0, screen_width, screen_height, 0, screen_depth, InputOutput, visual, CWCursor | CWEventMask, &window_attributes);

    font = XLoadFont(display, "-*-*-*-r-*-*-13-*-*-*-*-*-*-*");
    font_struct = XQueryFont(display, font);

    cursor = XCreateFontCursor(display, XC_arrow);
    hand_cursor = XCreateFontCursor(display, XC_hand1);
    wait_cursor = XCreateFontCursor(display, XC_watch);

    colormap = XDefaultColormapOfScreen(screen);
	color_move_window = objects_color(0, 0, 65535);
	color_resize_window = objects_color(30000, 30000, 0);
	color_minimize_window = objects_color(0, 0, 30000);
	color_maximize_window = objects_color(30000, 0, 30000);
	color_close_window = objects_color(65535, 0, 0);
    color_category_button = objects_color(0, 40000, 0);
	color_back_menu = objects_color(40000, 40000, 40000);
	color_battery_ok = objects_color(0, 65535, 0);
	color_battery_ko = objects_color(65535, 0, 0);
	color_bar = objects_color(0, 65535, 0);

    XGCValues gcv_text_white = {
        .foreground = white_pixel,
        .font = font
    };
	gc_text_white = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_white);
    XGCValues gcv_text_black = {
        .foreground = black_pixel,
        .font = font
    };
	gc_text_black = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_black);

    XGCValues gcv_category_button = { .foreground = color_category_button.pixel };
    gc_category_button = XCreateGC(display, window, GCForeground, &gcv_category_button);

	XGCValues gcv_icon = { .foreground = color_back_menu.pixel };
    gc_icon = XCreateGC(display, window, GCForeground, &gcv_icon);

	XGCValues gcv_bar = { .foreground = color_bar.pixel };
	gc_bar = XCreateGC(display, window, GCForeground, &gcv_bar);

	XGCValues gcv_splash_rectangle = { .foreground = white_pixel };
	gc_splash_rectangle = XCreateGC(display, window, GCForeground, &gcv_splash_rectangle);

	XColor color_taskbar = objects_color(65535, 65535, 65535);
    XGCValues gcv_taskbar = { .foreground = color_taskbar.pixel };
    gc_taskbar = XCreateGC(display, window, GCForeground, &gcv_taskbar);

    XColor color_taskbar_button = objects_color(65535, 35000, 10000);
    XGCValues gcv_taskbar_button = { .foreground = color_taskbar_button.pixel };
    gc_taskbar_button = XCreateGC(display, window, GCForeground, &gcv_taskbar_button);

	XColor color_taskbar_button_focus = objects_color(20000, 50000, 20000);
    XGCValues gcv_taskbar_button_focus = { .foreground = color_taskbar_button_focus.pixel };
    gc_taskbar_button_focus = XCreateGC(display, window, GCForeground, &gcv_taskbar_button_focus);

    XColor color_taskbar_button_hidden = objects_color(20000, 20000, 20000);
    XGCValues gcv_taskbar_button_hidden = { .foreground = color_taskbar_button_hidden.pixel };
    gc_taskbar_button_hidden = XCreateGC(display, window, GCForeground, &gcv_taskbar_button_hidden);

    XGCValues gcv_taskbar_button_border = { .foreground = color_taskbar.pixel, .background = black_pixel };
    gc_taskbar_button_border = XCreateGC(display, window, GCForeground | GCBackground, &gcv_taskbar_button_border);

	XGCValues gcv_toolbar_button = (XGCValues) { .foreground = white_pixel };
	gc_toolbar_button = XCreateGC(display, window, GCForeground, &gcv_toolbar_button);

	XGCValues values = { .foreground = color_back_menu.pixel };
    gc_menu = XCreateGC(display, window, GCForeground, &values);

	gc_battery = XCreateGC(display, window, 0, NULL);
}

XColor objects_color(unsigned short r, unsigned short g, unsigned short b) {
	XColor color = {
		.red = r,
		.green = g,
		.blue = b
	};

	XAllocColor(display, colormap, &color);

	return color;
}
