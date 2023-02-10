#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/keysym.h>
#include "event.h"
#include "objects.h"
#include "debug.h"
#include "window.h"
#include "rooibos.h"
#include "taskbar.h"
#include "toolbar.h"
#include "cache.h"
#include "icon.h"
#include "menu.h"
#include "prompt.h"
#include "utils.h"

char mode = ' ';
window_t *window_focus = NULL;
int moving = 0;
int resizing = 0;

int should_reorganize = 0;

cairo_surface_t *wallpaper_surface = NULL;

void event_reorganize(void) {
    switch (windows_get()->length) {
        case 1:
            XMoveResizeWindow(display, windows_get()->first->id, 0, 0, screen_width, screen_height - 100);
            break;
        case 2:
            XMoveResizeWindow(display, windows_get()->first->id, 0, 0, screen_width / 2, screen_height - 100);
            XMoveResizeWindow(display, windows_get()->first->next->id, screen_width / 2, 0, screen_width / 2, screen_height - 100);
            break;
        case 3:
            XMoveResizeWindow(display, windows_get()->first->id, 0, 0, screen_width / 2, (screen_height - 100) / 2);
            XMoveResizeWindow(display, windows_get()->first->next->id, screen_width / 2, 0, screen_width / 2, (screen_height - 100) / 2);
            XMoveResizeWindow(display, windows_get()->first->next->next->id, 0, (screen_height - 100) / 2 , screen_width, (screen_height - 100) / 2);
            break;
        case 4:
            XMoveResizeWindow(display, windows_get()->first->id, 0, 0, screen_width / 2, (screen_height - 100) / 2);
            XMoveResizeWindow(display, windows_get()->first->next->id, screen_width / 2, 0, screen_width / 2, (screen_height - 100) / 2);
            XMoveResizeWindow(display, windows_get()->first->next->next->id, 0, (screen_height - 100) / 2 , screen_width / 2, (screen_height - 100) / 2);
            XMoveResizeWindow(display, windows_get()->first->next->next->next->id, screen_width / 2, (screen_height - 100) / 2 , screen_width / 2, (screen_height - 100) / 2);
            break;
    }

    should_reorganize = 0;
}

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
            event_on_key_press(event.xkey, event.xkey.keycode);
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
    show_wallpaper();

    if (menu.is_showed == 1) {
        menu_show();
    } else {
        taskbar_show();
        toolbar_show();
    }

    if (should_reorganize == 1) {
        event_reorganize();
    }

    if (prompt_active == 1) {
        prompt_show();
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
            icons_on_press(x, y);

            int category_button = menu_category_buttons_on_hover(x, y);
            menu_category_buttons_on_press(category_button);
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

void event_on_key_press(XKeyEvent key_event, int key) {
    KeySym key_sym = XLookupKeysym(&key_event, 0);

    if (prompt_active == 1) {    
        prompt_on_key_press(key_event, key_sym);
    } else if (key_sym == XK_Super_L) {
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
    } else if (key_sym == XK_Left) {
        if (menu.is_showed == 1 && menu.category_index > -1) {
            menu_go_to_previous_category();
        }
    } else if (key_sym == XK_Right) {
        if (menu.is_showed == 1 && menu.category_index < 8) {
            menu_go_to_next_category();
        }
    } else if (key_sym == XK_Escape) {
        if (menu.is_showed == 1) {
            menu_clear();
            window_show_all_visible();
            taskbar_show();
            toolbar_show();
        }
    } else if (key_sym == XK_Control_L) {
        if (prompt_active == 0 && menu.is_showed == 0) {
            prompt_show();
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
            menu_category_buttons_on_hover(x, y);
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
    XMapWindow(display, child);

    if (menu.is_showed == 1) {
        menu_clear();
        window_show_all_visible();
        taskbar_show();
        toolbar_show();
        should_reorganize = 1;
    }

    char *title;
    XFetchName(display, child, &title);

    if (title != NULL) {
        if (window_get(child) == NULL) {
            XSelectInput(display, child, PropertyChangeMask);
            window_t *new_window_ = window_init(child, title, x, y, width, height);
            window_add(new_window_);
            taskbar_update_windows();
            should_reorganize = 1;
            XEvent event = { .type = Expose };
            XSendEvent(display, window, 0, ExposureMask, &event);
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
    if (menu.is_showed == 0) {
        taskbar_show();
    }
}

void event_on_destroy(Window child) {
    window_focus = NULL;
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
    event_reorganize();
}

void show_wallpaper(void) {
    if (wallpaper_surface == NULL) {
        char *wallpaper_file_path = malloc(sizeof(char) * 200);
        strcpy(wallpaper_file_path, "");
        if (access("/usr/share/rooibos/wallpaper.png", F_OK) == 0) {
            strcpy(wallpaper_file_path, "/usr/share/rooibos/wallpaper.png");
        }

        char *wallpaper_path = utils_get(UTILS_WALLPAPER);
        if (access(wallpaper_path, F_OK) == 0) {
            strcpy(wallpaper_file_path, wallpaper_path);
        }
        free(wallpaper_path);
        
        if (access("./assets/wallpaper.png", F_OK) == 0) {
            strcpy(wallpaper_file_path, "./assets/wallpaper.png");
        }

        wallpaper_surface = icon_get_surface_png(wallpaper_file_path);

        free(wallpaper_file_path);
    }
    icon_draw_png(wallpaper_surface, "", 0, 0, screen_width, screen_height);
}
