#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <unistd.h>
#include "rooibos.h"
#include "window.h"
#include "taskbar.h"
#include "toolbar_button.h"

void rooibos(void) {
	Display *display = XOpenDisplay(NULL);
	Screen *screen = XDefaultScreenOfDisplay(display);
	const Window root_window = XDefaultRootWindow(display);
	Visual *visual = XDefaultVisualOfScreen(screen);
	const unsigned long white_pixel = XWhitePixelOfScreen(screen);
	const unsigned long black_pixel = XBlackPixelOfScreen(screen);

	const int screen_width = XWidthOfScreen(screen);
	const int screen_height = XHeightOfScreen(screen);
	const int screen_depth = XDefaultDepthOfScreen(screen);

	const Cursor cursor = XCreateFontCursor(display, XC_arrow);

	XSetWindowAttributes window_attributes;
	window_attributes.background_pixel = black_pixel;
	window_attributes.cursor = cursor;
	window_attributes.event_mask = ExposureMask | ButtonPressMask | PointerMotionMask;
	const Window window = XCreateWindow(display, root_window, 0, 0, screen_width, screen_height, 0, screen_depth, InputOutput, visual, CWBackPixel | CWCursor | CWEventMask, &window_attributes);
	XStoreName(display, window, "rooibos");

	XSelectInput(display, root_window, SubstructureNotifyMask);

	XGCValues gcv;
	gcv.foreground = white_pixel;
	const GC gc_button = XCreateGC(display, window, GCForeground, &gcv);

	XMapWindow(display, window);
	XFlush(display);

	FILE *debug = fopen("./debug.txt", "w");

	int quit = 0;

	const Colormap colormap = XDefaultColormapOfScreen(screen);
	XColor red;
	red.red = 60000;
	XAllocColor(display, colormap, &red);

	XGCValues gcv_red;
	gcv_red.foreground = red.pixel;
	const GC gc_red = XCreateGC(display, window, GCForeground, &gcv_red);

	XGCValues gcv_bar;
	gcv_bar.foreground = white_pixel;
	GC gc_bar = XCreateGC(display, window, GCForeground, &gcv_bar);

	const Font font_text_button = XLoadFont(display, "-*-times-*-r-*-*-14-*-*-*-*-*-*-*");
	XGCValues gcv_text_button;
	gcv_text_button.foreground = black_pixel;
	gcv_text_button.font = font_text_button;
	const GC gc_text_button = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_button);

	const Font font_text_button_toolbar = XLoadFont(display, "-*-times-*-r-*-*-14-*-*-*-*-*-*-*");
	XGCValues gcv_text_button_toolbar;
	gcv_text_button_toolbar.foreground = white_pixel;
	gcv_text_button_toolbar.font = font_text_button_toolbar;
	const GC gc_text_button_toolbar = XCreateGC(display, window, GCForeground | GCFont, &gcv_text_button_toolbar);

	struct taskbar *tb = taskbar_init(display, window, screen, screen_width, screen_height);
	windows_init();

	char mode = ' ';
	struct window *window_focus = NULL;
	int moving = 0;
	int resizing = 0;

	struct toolbar_button tbb[7];

	XEvent event;
	while (1) {
		XNextEvent(display, &event);

		switch (event.type) {
			case Expose:
				taskbar_refresh(display, window, tb);

				XFontStruct *fontstruct = XQueryFont(display, font_text_button_toolbar);

				int button_width = screen_width / 7;
				char *labels[7] = { "New window", "Move window", "Resize window", "Minimize window", "Maximize window", "Close window", "Exit" };
				for (int i = 0; i < 7; i++) {
					int width = XTextWidth(fontstruct, labels[i], strlen(labels[i]));
					int x = (button_width - width) / 2;

					struct toolbar_button new_tbb = { labels[i], button_width * i, screen_height - 100, button_width, 50 };
					tbb[i] = new_tbb;
					XDrawRectangle(display, window, gc_button, button_width * i, screen_height - 100, button_width, 50);
					XDrawString(display, window, gc_text_button_toolbar, (button_width * i) + x, screen_height - 70, labels[i], strlen(labels[i]));
				}

				break;
			case ButtonPress: {
				int x = event.xbutton.x;
				int y = event.xbutton.y;

				if (x >= tb->x && x <= screen_width && y >= tb->y && y <= screen_height) {
					for (int i = 0; i < tb->buttons_length; i++) {
						if (x >= tb->tb_buttons[i]->x &&
							x <= tb->tb_buttons[i]->x + tb->tb_buttons[i]->width &&
							y >= tb->tb_buttons[i]->y &&
							y <= tb->tb_buttons[i]->y + tb->tb_buttons[i]->height) {
								if (tb->tb_buttons[i]->window->visible == 1) {
									window_focus = NULL;
									XUnmapWindow(display, tb->tb_buttons[i]->window->id);
									tb->tb_buttons[i]->window->visible = 0;
								} else {
									window_focus = tb->tb_buttons[i]->window;
									XMapWindow(display, tb->tb_buttons[i]->window->id);
									tb->tb_buttons[i]->window->visible = 1;
								}
							}
					}
				} else if (y >= screen_height - 100 && y <= screen_height - 50) {
					int position = ((double) x / (double) screen_width) * 7;

					switch (position) {
						case 0:
							new_window();
							break;
						case 1:
							mode = 'm';
							break;
						case 2:
							mode = 'r';
							break;
						case 3:
							if (window_focus != NULL) {
								XUnmapWindow(display, window_focus->id);
								window_focus->visible = 0;
							}
							break;
						case 4:
							if (window_focus != NULL) {
								XMoveResizeWindow(display, window_focus->id, 0, 0, screen_width, screen_height - 100);
							}
							break;
						case 5:
							if (window_focus != NULL) {
								XDestroyWindow(display, window_focus->id);
								window_focus = NULL;
							}
							break;
						case 6:
							quit = 1;
							break;
					}
				}

				if (mode != ' ' && (moving == 1 || resizing == 1)) {
					moving = 0;
					resizing = 0;
					mode = ' ';
				}

				break;
			}
			case MotionNotify: {
				if (mode == 'm' && window_focus != 0) {
					moving = 1;
				}
				if (moving == 1) {
					XMoveWindow(display, window_focus->id, event.xmotion.x + 10, event.xmotion.y + 10);
				}

				if (mode == 'r' && window_focus != 0) {
					resizing = 1;
				}
				if (resizing == 1) {
					XResizeWindow(display, window_focus->id, (event.xmotion.x - window_focus->x) - 10, (event.xmotion.y - window_focus->y) - 10);
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
						window_focus = window_get(child);
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
				window_focus = NULL;
				moving = 0;
				mode = ' ';
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

void new_window(void) {
	if (fork() == 0) {
		execlp("xterm", "xterm", NULL);
	}
}