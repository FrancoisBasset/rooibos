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

GC gc_text_white;
GC gc_text_black;
GC gc_category_button;
GC gc_icon;

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
	color_move_window = (XColor) { .red = 0, .green = 0, .blue = 65535 };
	color_resize_window = (XColor) { .red = 30000, .green = 30000, .blue = 0 };
	color_minimize_window = (XColor) { .red = 0, .green = 0, .blue = 30000 };
	color_maximize_window = (XColor) { .red = 30000, .green = 0, .blue = 30000 };
	color_close_window = (XColor) { .red = 65535, .green = 0, .blue = 0 };
    color_category_button = (XColor) { .red = 0, .green = 40000, .blue = 0 };
	color_back_menu = (XColor) { .red = 40000, .green = 40000, .blue = 40000 };

	XAllocColor(display, colormap, &color_move_window);
	XAllocColor(display, colormap, &color_resize_window);
	XAllocColor(display, colormap, &color_minimize_window);
	XAllocColor(display, colormap, &color_maximize_window);
	XAllocColor(display, colormap, &color_close_window);
    XAllocColor(display, colormap, &color_category_button);
    XAllocColor(display, colormap, &color_back_menu);

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
}
