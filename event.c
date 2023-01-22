#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "event.h"
#include "objects.h"
#include "window.h"
#include "rooibos.h"
#include "taskbar.h"
#include "toolbar.h"
#include "cache.h"
#include "icon.h"
#include "menu.h"

char mode = ' ';
window_t *window_focus = NULL;
int moving = 0;
int resizing = 0;

cairo_surface_t *wallpaper_surface = NULL;

int handle_event(void) {
    int quit = 0;

    XEvent event;
    XNextEvent(display, &event);

    switch (event.type) {
        case Expose:
            event_on_expose();
            break;
        case ButtonPress: {
            quit = event_on_button_press(event.xbutton.button, event.xbutton.x, event.xbutton.y);
            break;
        }
        case KeyPress: {
            if (event.xkey.keycode == 133) {
                if (menu.is_showed == 0) {
                    XDefineCursor(display, window, wait_cursor);
                    XFlush(display);
                    icons_init();
                    XDefineCursor(display, window, cursor);
                    XFlush(display);
                    menu_show();
                    taskbar_show();
                    toolbar_show();
                } else {
                    menu_clear();
                    taskbar_hide();
                    toolbar_hide();
                    XDefineCursor(display, window, cursor);
                }
            }
            break;
        }
        case MotionNotify: {
            event_on_motion(event.xmotion.x, event.xmotion.y);
            break;
        }
        case ConfigureNotify: {
            event_on_configure(event.xconfigure.window, event.xconfigure.x, event.xconfigure.y, event.xconfigure.width, event.xconfigure.height);
            break;
        }
        case PropertyNotify: {
            event_on_property(event.xproperty.window);
            break;
        }
        case DestroyNotify: {
            event_on_destroy(event.xconfigure.window);
            break;
        }
        default:
            fprintf(debug, "Event %d not implemented !\n", event.type);
            break;
    }

    XFlush(display);

    return quit;
}

void event_on_expose(void) {
    if (wallpaper_surface == NULL) {
        wallpaper_surface = icon_get_surface_png("tux.png");
    }
    icon_draw_png(wallpaper_surface, "", 0, 0, screen_width, screen_height);

    if (menu.is_showed) {
        menu_show();
        taskbar_show();
        toolbar_show();
    }
}

int event_on_button_press(int button, int x, int y) {
    int quit = 0;

    if (button == 1) {
        quit = event_on_left_button_press(x, y);
    } else if (button == 3) {
        quit = event_on_right_button_press(x, y);
    }

    return quit;
}

int event_on_left_button_press(int x, int y) {
    int quit = 0;

    if (menu.is_showed && taskbar_is_pressed(x, y)) {
        taskbar_on_press(x, y);
    } else if (menu.is_showed && toolbar_is_hover(y)) {
        quit = toolbar_on_click(x);
    }

    if (mode != ' ' && (moving == 1 || resizing == 1)) {
        moving = 0;
        resizing = 0;
        mode = ' ';
    }

    if (menu.is_showed == 1) {
        icons_on_click(x, y);
    }

    return quit;
}

int event_on_right_button_press(int x, int y) {
    return 0;
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
            if (toolbar_is_hover(y) == 1) {
                XDefineCursor(display, window, hand_cursor);
            } else {
                XDefineCursor(display, window, cursor);
            }
        }        
    }
}

void event_on_configure(Window child, int x, int y, int width, int height) {
    XMapWindow(display, child);

    if (y < 100 && menu.is_showed == 1) {
        icons_show();
    }

    char *title;
    XFetchName(display, child, &title);

    if (title != NULL) {
        if (window_get(child) == NULL) {
            XSelectInput(display, child, PropertyChangeMask);
            XMoveWindow(display, child, 200, 200);
            window_t *new_window_ = window_init(child, title, x, y, width, height);
            window_add(new_window_);
            taskbar_update_windows();
        } else {
            window_focus = window_get(child);
            window_update(child, "title", title);
            window_update(child, "x", &x);
            window_update(child, "y", &y);
            window_update(child, "width", &width);
            window_update(child, "height", &height);
        }
    }
}

void event_on_property(Window child) {
    char *title;
    XFetchName(display, child, &title);
    window_update(child, "title", title);
    if (menu.is_showed) {
        taskbar_show();
    }
}

void event_on_destroy(Window child) {
    window_focus = NULL;
    moving = 0;
    mode = ' ';
    window_delete(child);
    taskbar_update_windows();
    if (menu.is_showed) {
        taskbar_show();
    }
}

void new_window(void) {
	if (fork() == 0) {
		execlp("xterm", "xterm", NULL);
	}
}
