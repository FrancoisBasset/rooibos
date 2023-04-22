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
#include "toolbar.h"
#include "cache.h"
#include "icon.h"
#include "menu.h"
#include "prompt.h"
#include "utils.h"
#include "sound.h"
#include "brightness.h"

char mode = ' ';
window_t *window_focus = NULL;
int moving = 0;
int resizing = 0;
char *title_launched = NULL;

int sound_is_show = 0;
int brightness_is_show = 0;

Pixmap wallpaper_surface = -1;
int first_menu_show = 0;

int handle_event(void) {
    int quit = 0;

    XEvent event;
    XNextEvent(display, &event);

    switch (event.type) {
        case Expose:
            event_on_expose();
            break;
        case ButtonPress:
            quit = event_on_button_press(event.xbutton.button, event.xbutton.x, event.xbutton.y);
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
    	show_wallpaper(0);
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
        toolbar_show();
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

    if (menu.is_showed == 0 && taskbar_is_pressed(x, y)) {
        taskbar_on_press(x, y);
    } else if (menu.is_showed == 0 && prompt_active == 0 && toolbar_is_hover(y)) {
        quit = toolbar_on_press(x);
    }

    if (mode != ' ' && (moving == 1 || resizing == 1)) {
        moving = 0;
        resizing = 0;
        mode = ' ';
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
			}
        } else {
            menu_clear();
            window_show_all_visible();
            taskbar_show();
            toolbar_show();
        }
    }

    return quit;
}

int event_on_right_button_press(int x, int y) {
    return 0;
}

void event_on_key_press(XKeyEvent key_event) {
    KeySym key_sym = XLookupKeysym(&key_event, 0);

    if (prompt_active == 1) {    
        prompt_on_key_press(key_event, key_sym);
		return;
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
				toolbar_hide();
				menu_show();
			} else {
				menu_clear();
				window_show_all_visible();
				taskbar_show();
				toolbar_show();
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
				toolbar_show();
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
    if (mode == 'm' && window_focus != 0) {
        moving = 1;
        XMoveWindow(display, window_focus->id, x + 10, y + 10);
    }

    if (mode == 'r' && window_focus != 0) {
        resizing = 1;
        XResizeWindow(display, window_focus->id, (x - window_focus->x) - 10, (y - window_focus->y) - 10);
    }
    
    if (menu.is_showed == 1) {
        if (icons_on_hover(x, y) == 0) {
            if (menu_category_buttons_on_hover(x, y) == 0 && menu_buttons_on_hover(x, y) == 0) {
			    XDefineCursor(display, window, cursor);
			}
        }
    } else {
        if (prompt_active == 0 && toolbar_is_hover(y) == 1) {
            XDefineCursor(display, window, hand_cursor);
        } else {
            XDefineCursor(display, window, cursor);
        }        
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
        toolbar_show();
    }

    if (window_get(child) == NULL) {
		XMapWindow(display, child);
		XSelectInput(display, child, PropertyChangeMask | KeyPressMask | FocusChangeMask | EnterWindowMask);
		XGrabKey(display, XKeysymToKeycode(display, XK_Super_L), AnyModifier, child, True, GrabModeSync, GrabModeSync);

		window_t *new_window_ = window_init(child, title, x, y, width, height);
		
		window_add(new_window_);
		taskbar_update_windows();
		XMoveResizeWindow(display, child, 0, 0, screen_width, screen_height - 100);
	} else {
		window_focus = window_get(child);
		if (strcmp(title, "") != 0) {
			window_update(child, "title", title);
		}

		window_update(child, "x", &x);
		window_update(child, "y", &y);
		window_update(child, "width", &width);
		window_update(child, "height", &height);
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
    moving = 0;
    mode = ' ';
    window_delete(child);
    taskbar_update_windows();
    if (menu.is_showed == 0) {
        taskbar_show();
    }
}

void new_window(void) {
	if (fork() == 0) {
		execlp("xterm", "xterm", NULL);
	}
}

void show_wallpaper(int splash) {
	int height = screen_height - 100;
	if (menu.is_showed == 1 || splash == 1) {
		height = screen_height;
	}

    if (wallpaper_surface == -1) {
		char *wallpaper_to_use = utils_get(UTILS_WALLPAPER_TO_USE);
		wallpaper_surface = icon_get_pixmap(wallpaper_to_use, screen_width, height);
		free(wallpaper_to_use);
    }

	XCopyArea(display, wallpaper_surface, window, XDefaultGCOfScreen(screen), 0, 0, screen_width, height, 0, 0);
}
