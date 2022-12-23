#include <stdio.h>
#include <unistd.h>
#include "event.h"
#include "objects.h"
#include "window.h"
#include "rooibos.h"
#include "taskbar.h"
#include "toolbar.h"

int handle_event(void) {
    int quit = 0;

    XEvent event;
    XNextEvent(display, &event);

    switch (event.type) {
        case Expose:
            on_expose();
            break;
        case ButtonPress: {
            quit = on_button_press(event.xbutton.x, event.xbutton.y);
            break;
        }
        case MotionNotify: {
            on_motion(event.xmotion.x, event.xmotion.y);
            break;
        }
        case ConfigureNotify: {
            on_configure(event.xconfigure.window, event.xconfigure.x, event.xconfigure.y, event.xconfigure.width, event.xconfigure.height);
            break;
        }
        case DestroyNotify: {
            on_destroy(event.xconfigure.window);
            break;
        }
        default:
            fprintf(debug, "Event %d not implemented !\n", event.type);
            break;
    }

    XFlush(display);

    return quit;
}

void on_expose(void) {
    taskbar_refresh();
    toolbar_refresh();
}

int on_button_press(int x, int y) {
    //Taskbar
    if (x >= tb->x && x <= screen_width && y >= tb->y && y <= screen_height) {
        for (int i = 0; i < tb->buttons_length; i++) {
            struct taskbar_button *button = tb->tb_buttons[i];

            if (x >= button->x &&
                x <= button->x + button->width &&
                y >= button->y &&
                y <= button->y + button->height) {
                    if (button->window->visible == 1) {
                        window_focus = NULL;
                        XUnmapWindow(display, button->window->id);
                        button->window->visible = 0;
                    } else {
                        window_focus = button->window;
                        XMapWindow(display, button->window->id);
                        button->window->visible = 1;
                    }
                }
        }
    //Toolbar
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
                return 1;
        }
    }

    if (mode != ' ' && (moving == 1 || resizing == 1)) {
        moving = 0;
        resizing = 0;
        mode = ' ';
    }

    return 0;
}

void on_motion(int x, int y) {
    if (mode == 'm' && window_focus != 0) {
        moving = 1;
    }
    if (moving == 1) {
        XMoveWindow(display, window_focus->id, x + 10, y + 10);
    }

    if (mode == 'r' && window_focus != 0) {
        resizing = 1;
    }
    if (resizing == 1) {
        XResizeWindow(display, window_focus->id, (x - window_focus->x) - 10, (y - window_focus->y) - 10);
    }
}

void on_configure(Window child, int x, int y, int width, int height) {
    XMapWindow(display, child);

    char *title;
    XFetchName(display, child, &title);

    if (title != NULL) {
        if (window_get(child) == NULL) {
            XMoveWindow(display, child, 200, 200);
            struct window *new_window = window_init(child, title, x, y, width, height);
            window_add(new_window);
            taskbar_update_windows();
            taskbar_refresh();
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

void on_destroy(Window child) {
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