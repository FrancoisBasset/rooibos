#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include "event.h"
#include "objects.h"
#ifdef WILLDEBUG
#include "debug.h"
#endif
#include "window.h"
#include "rooibos.h"
#include "taskbar.h"
#include "cache.h"
#include "icon.h"
#include "menu.h"
#include "prompt.h"
#include "utils.h"
#include "sound.h"
#include "brightness.h"
#include "decorator.h"
#include "right_click_panel.h"
#include "wallpaper.h"
#include "shortcut.h"

Pixmap back = -1;
window_t *window_focus = NULL;
char *title_launched = NULL;

int sound_is_show = 0;
int brightness_is_show = 0;

int first_menu_show = 0;

int quit = 0;

int handle_event(void) {
	quit = 0;

	XEvent event;
	XNextEvent(display, &event);

	switch (event.type) {
		case Expose:
			event_on_expose();
			wallpaper_expose();
			break;
		case ButtonRelease:
			decorator_set_selected_null();
			break;
		case ButtonPress:
			if (event.xbutton.window == wallpaper_window) {
				wallpaper_on_press(event.xbutton.x, event.xbutton.y);
			} else {
				quit = event_on_button_press(event.xbutton.button, event.xbutton.x, event.xbutton.y);
			}
			break;
		case KeyPress:
			event_on_key_press(event.xkey);
			break;
		case MotionNotify:
			event_on_motion(event.xmotion.x, event.xmotion.y);
			break;
		case MapNotify:
		case ConfigureNotify:
			event_on_configure(event.xconfigure.window, event.xconfigure.x, event.xconfigure.y, event.xconfigure.width, event.xcreatewindow.height);
			break;
		case PropertyNotify:
			event_on_property(event.xproperty.window);
			break;
		case DestroyNotify:
			event_on_destroy(event.xdestroywindow.window);
			break;
		case FocusIn:
			window_focus = window_get(event.xfocus.window);
			taskbar_update_windows();
			break;
		case EnterNotify: {
			window_t *new_window_focus = window_get(event.xcrossing.window);
			if (strcmp(new_window_focus->title, "xterm") == 0 || strcmp(new_window_focus->title, "uxterm") == 0) {
				window_focus = new_window_focus;
				taskbar_update_windows();
				XEvent e = { .type = Expose };
				XSendEvent(display, window, 0, ExposureMask, &e);
			}
			break;
		}
		case 65:
			break;
		default:
#ifdef WILLDEBUG
			debug("Event not implemented : %d", event.type);
#endif
			break;
	}

	XFlush(display);

	return quit;
}

void event_on_expose(void) {
	if (menu.is_showed == 0 || first_menu_show == 0) {
		back = XCreatePixmap(display, window, screen_width, screen_height, screen_depth);
		wallpaper_show(0);
		shortcut_show_all();

		int height = screen_height - 50;
		if (menu.is_showed == 1) {
			height = screen_height;
		}

		XCopyArea(display, back, window, XDefaultGCOfScreen(screen), 0, 0, screen_width, height, 0, 0);
		XFreePixmap(display, back);
	}

	if (menu.is_showed == 0) {
		if (sound_is_show == 1) {
			sound_show();
			sound_is_show = 0;
		}
		if (brightness_is_show == 1) {
			brightness_show();
			brightness_is_show = 0;
		}
	}

	first_menu_show = menu.is_showed;

	if (menu.is_showed == 1) {
		menu_show();
	} else {
		taskbar_show();
		decorator_show_all();
	}

	if (prompt_active == 1) {
		prompt_show();
	}
}

int event_on_button_press(int button, int x, int y) {
	switch (button) {
		case 1:
			return event_on_left_button_press(x, y);
		case 3:
			return event_on_right_button_press(x, y);
		case 4:
		case 5:
			if (menu.is_showed) {
				menu_on_scroll(button, x, y);
			}
	}

	return 0;
}

int event_on_left_button_press(int x, int y) {
	int quit = 0;

	quit = right_click_panel_on_press(x, y);
	right_click_panel_hide();

	if (menu.is_showed == 0 && taskbar_is_pressed(x, y)) {
		taskbar_on_press(x, y);
	} else if (menu.is_showed == 0 && decorator_on_hover(x, y) != 0) {
		decorator_on_press(x, y);

		XEvent e = { .type = Expose };
		XSendEvent(display, window, 0, ExposureMask, &e);
	} else if (menu.is_showed == 0 && shortcut_on_hover(x, y) != -1) {
		shortcut_on_press(x, y);

		XEvent e = { .type = Expose };
		XSendEvent(display, window, 0, ExposureMask, &e);
	}

	if (menu.is_showed == 1) {
		if (x >= menu.x && x <= menu.x + menu.width && y >= menu.y && y <= menu.y + menu.height) {
			if (menu_apps_is_hover(x, y)) {
				icons_on_press(x, y);
			}

			int category_button = menu_category_buttons_on_hover(x, y);
			menu_category_buttons_on_press(category_button);

			int menu_button = menu_buttons_on_hover(x, y);
			if (menu_button == 1) {
				new_window();
			} else if (menu_button == 2) {
				quit = 1;
			} else if (menu_button == 3) {
				quit = 2;
				menu_clear();
				XDefineCursor(display, window, cursor);
			}
		} else {
			menu_clear();
			window_show_all_visible();
			taskbar_show();
		}
	}

	return quit;
}

int event_on_right_button_press(int x, int y) {
	right_click_panel_show(x, y);
	return 0;
}

void event_on_key_press(XKeyEvent key_event) {
	KeySym key_sym = XLookupKeysym(&key_event, 0);

	if (prompt_active == 1) {
		prompt_on_key_press(key_event, key_sym);
		return;
	}

	if (key_event.window == wallpaper_window) {
		wallpaper_on_key_press(key_event);
	} 
	
	switch (key_sym) {
		case XK_Super_L:
			if (menu.is_showed == 0) {
				XDefineCursor(display, window, wait_cursor);
				XFlush(display);
				icons_init();
				XDefineCursor(display, window, cursor);
				XFlush(display);

				window_hide_all_visible();
				taskbar_hide();
				menu_show();
			} else {
				menu_clear();
				window_show_all_visible();
				taskbar_show();
				XDefineCursor(display, window, cursor);
			}
			break;
		case XK_Left:
			if (menu.is_showed == 1 && menu.category_index > -1) {
				menu_go_to_previous_category();
			}
			break;
		case XK_Right:
			if (menu.is_showed == 1 && menu.category_index < categories_length - 1) {
				menu_go_to_next_category();
			}
			break;
	 	case XK_Up:
			if (menu.is_showed == 1) {
				icon_scroll_up();
				XEvent e = { .type = Expose };
				XSendEvent(display, window, 0, ExposureMask, &e);
			}
			break;
		case XK_Down:
			if (menu.is_showed == 1) {
				icon_scroll_down();
				XEvent e = { .type = Expose };
				XSendEvent(display, window, 0, ExposureMask, &e);
			}
			break;
		case XK_Escape:
			if (menu.is_showed == 1) {
				menu_clear();
				window_show_all_visible();
				taskbar_show();
			} else {
				quit = 2;
				menu_clear();
				window_hide_all_visible();
				XDefineCursor(display, window, cursor);
			}
			break;
		case XK_Control_L:
			if (prompt_active == 0 && menu.is_showed == 0) {
				prompt_show();
			}
			break;
		case XF86XK_MonBrightnessUp: {
			brightness_up();
			brightness_is_show = 1;
			XEvent e = { .type = Expose };
			XSendEvent(display, window, 0, ExposureMask, &e);
			break;
		}
		case XF86XK_MonBrightnessDown: {
			brightness_down();
			brightness_is_show = 1;
			XEvent e = { .type = Expose };
			XSendEvent(display, window, 0, ExposureMask, &e);
			break;
		}
		case XF86XK_AudioLowerVolume: {
			sound_volume_down();
			sound_is_show = 1;
			XEvent e = { .type = Expose };
			XSendEvent(display, window, 0, ExposureMask, &e);
			break;
		}
		case XF86XK_AudioRaiseVolume: {
			sound_volume_up();
			sound_is_show = 1;
			XEvent e = { .type = Expose };
			XSendEvent(display, window, 0, ExposureMask, &e);
			break;
		}
	}
}

void event_on_motion(int x, int y) {
	if (menu.is_showed == 1) {
		if (icons_on_hover(x, y) == 0) {
			if (menu_category_buttons_on_hover(x, y) == 0 && menu_buttons_on_hover(x, y) == 0) {
				XDefineCursor(display, window, cursor);
			}
		}
	} else {
		const int decorator_hover = decorator_on_hover(x, y);
		if (decorator_hover == 5) {
			XDefineCursor(display, window, resize_cursor);
		} else if (decorator_hover != 0 && decorator_hover != 1) {
			XDefineCursor(display, window, hand_cursor);
		} else {
			XDefineCursor(display, window, cursor);
		}

		right_click_panel_on_hover(x, y);
		shortcut_on_hover(x, y);
	}
}

void event_on_configure(Window child, int x, int y, int width, int height) {
	char *title;
	XFetchName(display, child, &title);

	if (title == NULL || strcmp(title, "rooibos") == 0) {
		return;
	}

	if (strlen(title) == 0 && window_get(child) == NULL) {
		if (title_launched == NULL) {
			free(title);
			return;
		}
		title = malloc(sizeof(char) * (strlen(title_launched) + 1));
		strcpy(title, title_launched);
		free(title_launched);
		title_launched = NULL;
	}

	if (menu.is_showed == 1) {
		menu_clear();
		window_show_all_visible();
		taskbar_show();
	}

	if (window_get(child) == NULL) {
		XMapWindow(display, child);
		XSelectInput(display, child, PropertyChangeMask | KeyPressMask | FocusChangeMask | EnterWindowMask);
		XGrabKey(display, XKeysymToKeycode(display, XK_Super_L), AnyModifier, child, True, GrabModeSync, GrabModeSync);

		window_t *new_window_ = window_init(child, title, x, y, width, height);
		
		window_add(new_window_);
		taskbar_update_windows();
		XMoveResizeWindow(display, child, 0, 20, screen_width, screen_height - 70);
	} else {
		window_focus = window_get(child);
		if (strcmp(title, "") != 0) {
			window_update(child, "title", title);
		}

		if (x > 0 && x < screen_width) {
			window_update(child, "x", &x);
		}
		if (y > 0 && y < screen_height) {
			window_update(child, "y", &y);
		}
		if (width > 0 && width < screen_width) {
			window_update(child, "width", &width);
		}
		if (height > 0 && height < screen_height) {
			window_update(child, "height", &height);
		}
	}

	free(title);
}

void event_on_property(Window child) {
	char *title;
	XFetchName(display, child, &title);

	if (strcmp(title, "") != 0) {
		window_update(child, "title", title);
		if (menu.is_showed == 0) {
			taskbar_show();
		}
	}
}

void event_on_destroy(Window child) {
	const window_t *tmp = window_get(child);
	if (tmp == NULL) {
		return;
	}

	if (window_focus != NULL && window_focus->id == child) {
		window_focus = NULL;
	}
	
	window_delete(child);
	taskbar_update_windows();
	if (menu.is_showed == 0) {
		taskbar_show();
		XEvent e = { .type = Expose };
		XSendEvent(display, window, 0, ExposureMask, &e);
	}
}

void new_window(void) {
	if (fork() == 0) {
		execlp("xterm", "xterm", NULL);
	}
}
