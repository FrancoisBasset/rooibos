#include <stdio.h>
#include <unistd.h>
#include "event.h"
#include "objects.h"
#include "window.h"
#include "rooibos.h"
#include "taskbar.h"
#include "toolbar.h"

char mode = ' ';
struct window *window_focus = NULL;
int moving = 0;
int resizing = 0;

int handle_event(void) {
    int quit = 0;

    XEvent event;
    XNextEvent(display, &event);

    switch (event.type) {
        case Expose:
            event_on_expose();
            break;
        case ButtonPress: {
            quit = event_on_button_press(event.xbutton.x, event.xbutton.y);
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
    taskbar_refresh();
    toolbar_refresh();
}

int event_on_button_press(int x, int y) {
    int quit = 0;

    if (taskbar_is_pressed(x, y)) {
        taskbar_on_press(x, y);
    } else if (toolbar_is_pressed(y)) {
        quit = toolbar_on_click(x);
    }

    if (mode != ' ' && (moving == 1 || resizing == 1)) {
        moving = 0;
        resizing = 0;
        mode = ' ';
    }

    return quit;
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
}

void event_on_configure(Window child, int x, int y, int width, int height) {
    XMapWindow(display, child);

    char *title;
    XFetchName(display, child, &title);

    if (title != NULL) {
        if (window_get(child) == NULL) {
            XSelectInput(display, child, PropertyChangeMask);
            XMoveWindow(display, child, 200, 200);
            struct window *new_window = window_init(child, title, x, y, width, height);
            window_add(new_window);
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
    taskbar_refresh();
}

void event_on_destroy(Window child) {
    window_focus = NULL;
    moving = 0;
    mode = ' ';
    window_delete(child);
    taskbar_update_windows();
    taskbar_refresh();
}

void new_window(void) {
	if (fork() == 0) {
		execlp("xterm", "xterm", NULL);
	}
}