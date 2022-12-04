#include <stdio.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <X11/cursorfont.h>
#include <unistd.h>
#include "rooibos.h"
#include "window.h"
#include "taskbar.h"

void rooibos(void) {
	Display *display = XOpenDisplay(NULL);
	Screen *screen = XDefaultScreenOfDisplay(display);
	Window root_window = XDefaultRootWindow(display);
	Visual *visual = XDefaultVisualOfScreen(screen);
	unsigned long white_pixel = XWhitePixelOfScreen(screen);
	unsigned long black_pixel = XBlackPixelOfScreen(screen);

	int screen_width = XWidthOfScreen(screen);
	int screen_height = XHeightOfScreen(screen);
	int screen_depth = XDefaultDepthOfScreen(screen);

	Cursor cursor = XCreateFontCursor(display, XC_arrow);

	XSetWindowAttributes window_attributes;
	window_attributes.background_pixel = black_pixel;
	window_attributes.cursor = cursor;
	window_attributes.event_mask = ExposureMask | ButtonPressMask;
	Window window = XCreateWindow(display, root_window, 0, 0, screen_width, screen_height, 0, screen_depth, InputOutput, visual, CWBackPixel | CWCursor | CWEventMask, &window_attributes);
	XStoreName(display, window, "rooibos");

	XSelectInput(display, root_window, SubstructureNotifyMask);

	XGCValues gcv;
	gcv.foreground = white_pixel;
	GC gc_button = XCreateGC(display, window, GCForeground, &gcv);

	XMapWindow(display, window);
	XFlush(display);

	FILE *debug = fopen("./debug.txt", "w");

	int quit = 0;

	Colormap colormap = XDefaultColormapOfScreen(screen);
	XColor red;
	red.red = 60000;
	XAllocColor(display, colormap, &red);

	XGCValues gcv_red;
	gcv_red.foreground = red.pixel;
	GC gc_red = XCreateGC(display, window, GCForeground, &gcv_red);

	XGCValues gcv_bar;
	gcv_bar.foreground = white_pixel;
	GC gc_bar = XCreateGC(display, window, GCForeground, &gcv_bar);

	Font font_text_button = XLoadFont(display, "-*-times-*-r-*-*-14-*-*-*-*-*-*-*");
	XGCValues gcv_text_button;
	gcv_text_button.foreground = black_pixel;
	gcv_text_button.font = font_text_button;
	GC gc_text_button = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_button);

	struct taskbar *tb = taskbar_init(display, window, screen, screen_width, screen_height);
	windows_init();

	XEvent event;
	while (1) {
		XNextEvent(display, &event);

		switch (event.type) {
			case Expose:
				taskbar_refresh(display, window, tb);

				XFillRectangle(display, window, gc_button, 1850, 50, 100, 50);
				XDrawString(display, window, gc_text_button, 1865, 80, "xterm", 5);

				XFillRectangle(display, window, gc_button, 1850, 150, 100, 50);
				XDrawString(display, window, gc_text_button, 1850, 180, "Libreoffice", 11);
				
				XFillRectangle(display, window, gc_button, 1850, 250, 100, 50);
				XDrawString(display, window, gc_text_button, 1870, 280, "Exit", 4);
				break;
			case ButtonPress: {
				int x = event.xbutton.x;
				int y = event.xbutton.y;

				if (x >= 1850 && x <= 1950 && y >= 50 && y <= 100) {
					if (fork() == 0) {
						execlp("xterm", "xterm", NULL);
					}
				} else if (x >= 1850 && x <= 1950 && y >= 150 && y <= 200) {
					if (fork() == 0) {
						execlp("libreoffice", "libreoffice", NULL);
					}
				} else if (x >= 1850 && x <= 1950 && y >= 250 && y <= 300) {
					quit = 1;
				} else if (x >= tb->x && x <= screen_width && y >= tb->y && y <= screen_height) {
					for (int i = 0; i < tb->buttons_length; i++) {
						if (x >= tb->tb_buttons[i]->x &&
							x <= tb->tb_buttons[i]->x + tb->tb_buttons[i]->width &&
							y >= tb->tb_buttons[i]->y &&
							y <= tb->tb_buttons[i]->y + tb->tb_buttons[i]->height) {
								if (tb->tb_buttons[i]->window->visible == 1) {
									XUnmapWindow(display, tb->tb_buttons[i]->window->id);
									tb->tb_buttons[i]->window->visible = 0;
								} else {
									XMapWindow(display, tb->tb_buttons[i]->window->id);
									tb->tb_buttons[i]->window->visible = 1;
								}
							}
					}
				}

				break;
			}
			case ConfigureNotify: {
				Window child = event.xconfigure.window;
				XMapWindow(display, child);

				char *title;
				XFetchName(display, child, &title);
				int x = event.xconfigure.x;
				int y = event.xconfigure.y;
				int width = event.xconfigure.width;
				int height = event.xconfigure.height;

				if (title != NULL) {
					if (window_get(child) == NULL) {
						XMoveWindow(display, child, 200, 200);
						struct window *new_window = window_init(child, title, x, y, width, height);
						window_add(new_window);
						taskbar_set_windows(tb, windows_get());
						taskbar_refresh(display, window, tb);
					} else {
						window_update(child, "title", title);
						window_update(child, "x", &x);
						window_update(child, "y", &y);
						window_update(child, "width", &width);
						window_update(child, "height", &height);
					}
					
				}

				break;
			}
			case DestroyNotify: {
				Window child = event.xconfigure.window;
				window_delete(child);
				taskbar_set_windows(tb, windows_get());
				taskbar_refresh(display, window, tb);
				break;
			}
			default:
				fprintf(debug, "Event %d not implemented !\n", event.type);
				break;
		}

		if (quit) {
			break;
		}

		XFlush(display);
	}

	if (debug) {
		windows_print(debug);
		fclose(debug);
	}
	XCloseDisplay(display);
}
