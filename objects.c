#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include "objects.h"

Display *display;
Screen *screen;
Window root_window;
Window window;
unsigned long white_pixel;
unsigned long black_pixel;
int screen_width;
int screen_height;
Font font;
XFontStruct *font_struct;
Cursor cursor;
Cursor hand_cursor;
GC gc_text_black;
GC gc_text_white;
Colormap colormap;

void objects_init(void) {
    display = XOpenDisplay(NULL);
    screen = XDefaultScreenOfDisplay(display);
    root_window = XDefaultRootWindow(display);
    Visual *visual = XDefaultVisualOfScreen(screen);
    white_pixel = XWhitePixelOfScreen(screen);
    black_pixel = XBlackPixelOfScreen(screen);
    screen_width = XWidthOfScreen(screen);
    screen_height = XHeightOfScreen(screen);
    int screen_depth = XDefaultDepthOfScreen(screen);
    cursor = XCreateFontCursor(display, XC_arrow);
    hand_cursor = XCreateFontCursor(display, XC_hand1);

    font = XLoadFont(display, "-*-times-*-r-*-*-14-*-*-*-*-*-*-*");
    font_struct = XQueryFont(display, font);

    XSetWindowAttributes window_attributes = {
        .background_pixel = black_pixel,
        .cursor = cursor,
        .event_mask = ExposureMask | ButtonPressMask | PointerMotionMask | KeyPressMask
    };

    window = XCreateWindow(display, root_window, 0, 0, screen_width, screen_height, 0, screen_depth, InputOutput, visual, CWBackPixel | CWCursor | CWEventMask, &window_attributes);

    XGCValues gcv_text_black = {
        .foreground = black_pixel,
        .font = font
    };
	gc_text_black = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_black);

    XGCValues gcv_text_white = {
        .foreground = white_pixel,
        .font = font
    };
	gc_text_white = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_white);

    colormap = XDefaultColormapOfScreen(screen);
}
