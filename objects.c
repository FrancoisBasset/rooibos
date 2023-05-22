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
Cursor resize_cursor;

Colormap colormap;
XColor color_red;
XColor color_green;
XColor color_category_button;
XColor color_back_menu;
XColor color_decorator;
XColor color_decorator_min;
XColor color_decorator_max;
XColor color_right_panel_hover;
XColor color_wallpaper_modify_button;

GC gc_fore_red;
GC gc_fore_green;
GC gc_text_white;
GC gc_text_black;
GC gc_category_button;
GC gc_icon;
GC gc_splash_rectangle;
GC gc_taskbar;
GC gc_taskbar_button;
GC gc_taskbar_button_focus;
GC gc_taskbar_button_hidden;
GC gc_taskbar_button_border;
GC gc_menu;
GC gc_battery;
GC gc_decorator;
GC gc_decorator_min;
GC gc_decorator_max;
GC gc_right_panel_hover;
GC gc_wallpaper_modify_button;

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
		.event_mask = ExposureMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask | FocusChangeMask
	};
	window = XCreateWindow(display, root_window, 0, 0, screen_width, screen_height, 0, screen_depth, InputOutput, visual, CWCursor | CWEventMask, &window_attributes);

	font = XLoadFont(display, "-*-*-*-r-*-*-13-*-*-*-*-*-*-*");
	font_struct = XQueryFont(display, font);

	cursor = XCreateFontCursor(display, XC_arrow);
	hand_cursor = XCreateFontCursor(display, XC_hand1);
	wait_cursor = XCreateFontCursor(display, XC_watch);
	resize_cursor = XCreateFontCursor(display, XC_sizing);

	colormap = XDefaultColormapOfScreen(screen);
	color_red = objects_color(65535, 0, 0);
	color_green = objects_color(0, 65535, 0);
	color_category_button = objects_color(0, 40000, 0);
	color_back_menu = objects_color(40000, 40000, 40000);
	color_decorator = objects_color(10000, 20000, 30000);
	color_decorator_min = objects_color(65535, 20000, 20000);
	color_decorator_max = objects_color(0, 50000, 0);
	color_right_panel_hover = objects_color(30000, 50000, 50000);
	color_wallpaper_modify_button = objects_color(20000, 65535, 20000);

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

	XGCValues gcv_fore_green = { .foreground = color_green.pixel };
	gc_fore_green = XCreateGC(display, window, GCForeground, &gcv_fore_green);

	XGCValues gcv_splash_rectangle = { .foreground = white_pixel };
	gc_splash_rectangle = XCreateGC(display, window, GCForeground, &gcv_splash_rectangle);

	XGCValues gcv_taskbar = { .foreground = white_pixel };
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

	XGCValues gcv_taskbar_button_border = { .foreground = white_pixel, .background = black_pixel };
	gc_taskbar_button_border = XCreateGC(display, window, GCForeground | GCBackground, &gcv_taskbar_button_border);

	XGCValues values = { .foreground = color_back_menu.pixel };
	gc_menu = XCreateGC(display, window, GCForeground, &values);

	gc_battery = XCreateGC(display, window, 0, NULL);

	XGCValues gcv_decorator = { .foreground = color_decorator.pixel };
	gc_decorator = XCreateGC(display, window, GCForeground, &gcv_decorator);

	XGCValues gcv_decorator_min = { .foreground = color_decorator_min.pixel };
	gc_decorator_min = XCreateGC(display, window, GCForeground, &gcv_decorator_min);

	XGCValues gcv_decorator_max = { .foreground = color_decorator_max.pixel };
	gc_decorator_max = XCreateGC(display, window, GCForeground, &gcv_decorator_max);

	XGCValues gcv_fore_red = { .foreground = color_red.pixel };
	gc_fore_red = XCreateGC(display, window, GCForeground, &gcv_fore_red);

	XGCValues gcv_right_panel_hover = { .foreground = color_right_panel_hover.pixel };
	gc_right_panel_hover = XCreateGC(display, window, GCForeground, &gcv_right_panel_hover);

	XGCValues gcv_wallpaper_modify_button = { .foreground = color_wallpaper_modify_button.pixel };
	gc_wallpaper_modify_button = XCreateGC(display, window, GCForeground, &gcv_wallpaper_modify_button);
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
