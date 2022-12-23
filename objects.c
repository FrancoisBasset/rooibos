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
    Cursor cursor = XCreateFontCursor(display, XC_arrow);

    XSetWindowAttributes window_attributes = {
        .background_pixel = black_pixel,
        .cursor = cursor,
        .event_mask = ExposureMask | ButtonPressMask | PointerMotionMask
    };

    window = XCreateWindow(display, root_window, 0, 0, screen_width, screen_height, 0, screen_depth, InputOutput, visual, CWBackPixel | CWCursor | CWEventMask, &window_attributes);
}
