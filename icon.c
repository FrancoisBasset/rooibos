#include <math.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
#include "objects.h"
#include "icon.h"
#include "rooibos.h"
#include "appshortcut.h"
#include "cache.h"
#include "menu.h"
#include "event.h"

icon_t *icons;
int app_shortcuts_length = 0;
int already_hover = 0;
int icon_hover = -1;

int icon_width = 0;

void icon_draw(icon_t icon) {
	if (icon.pixmap == -1) {
		icon.pixmap = icon_get_pixmap(icon.filename, icon.width, icon.height);
	}

    XCopyArea(display, icon.pixmap, menu_pixmap, XDefaultGCOfScreen(screen), 0, 0, icon.width, icon.height, icon.x, icon.y);

	int name_width = XTextWidth(font_struct, icon.name, (int) strlen(icon.name));
    int name_length = (int) strlen(icon.name);

    while (name_width > icon.width) {
        name_length--;
        name_width = XTextWidth(font_struct, icon.name, name_length);
    }

    XDrawString(display, menu_pixmap, gc_text_white, icon.x, icon.y + icon.width + 10, icon.name, name_length);
}

Pixmap icon_get_pixmap(const char *filename, int width, int height) {
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(filename, width, height, 1, NULL);
	Pixmap pixmap = XCreatePixmap(display, window, width, height, screen_depth);

	gdk_pixbuf_xlib_render_to_drawable(pixbuf, pixmap, XDefaultGCOfScreen(screen), 0, 0, 0, 0, width, height, XLIB_RGB_DITHER_NONE, 0, 0);

	return pixmap;
}

void icons_init(void) {
    cache_update();
    appshortcut_t *app_shortcuts = cache_get_app_shortcuts(&app_shortcuts_length);

	categories_length = 0;
	for (int i = 0, j = 0; i < 9; i++) {
		int length = appshortcut_get_length_by_category(app_shortcuts, app_shortcuts_length, all_categories[i]);
		if (length > 0) {
			categories[j] = all_categories[i];
			categories_length++;
			j++;
		}
	}

    if (menu.category_index != -1) {
        app_shortcuts = appshortcut_get_app_shortcuts_by_category(app_shortcuts, categories[menu.category_index], app_shortcuts_length, &app_shortcuts_length);

		if (app_shortcuts_length == 0) {
			return;
		}
    }
    
    int x = 0;
    int y = 100;

    icons = malloc(sizeof(icon_t) * app_shortcuts_length);

    int nb_apps_in_line = 4;

    const int menu_width = (int) (menu.width * 0.60);
    const int menu_height = (int) (menu.height * 0.90);

    const int space_x = (menu.width - menu_width) / nb_apps_in_line;
    const int space_y = (menu.height - menu_height) / nb_apps_in_line;

    x += space_x;
    y += (space_y / 2);

    const int width = menu_width / nb_apps_in_line;
    const int height = menu_height / nb_apps_in_line;

    icon_width = width < height ? width : height;

    for (int i = 0; i < app_shortcuts_length; i++) {
        if (x >= menu.width - icon_width) {
            x = space_x;
            y += icon_width + space_y + (space_y / 2);
        }

		Pixmap pixmap = icon_get_pixmap(app_shortcuts[i].icon, icon_width, icon_width);
        icon_t new_icon = { app_shortcuts[i].icon, app_shortcuts[i].name, app_shortcuts[i].exec, x, y, menu.x + x, menu.y + y, icon_width, icon_width, pixmap };

        icons[i] = new_icon;

        x += icon_width + space_x;
    }

    /*for (int i = 0; i < app_shortcuts_length; i++) {
		free(app_shortcuts[i].name);
		free(app_shortcuts[i].exec);
		free(app_shortcuts[i].category);
		free(app_shortcuts[i].icon);
		free(app_shortcuts[i].file);
	}*/

	free(app_shortcuts);
}

void icons_draw(void) {
    for (int i = 0; i < app_shortcuts_length; i++) {
		if (icons[i].y > up_line_y && icons[i].y + icons[i].height < bottom_line_y) {
			icon_draw(icons[i]);
		}
    }
}

int icons_on_hover(int x, int y) {
    int is_hover = 0;

    for (int i = 0; i < app_shortcuts_length; i++) {
        if (x >= icons[i].x_press && x <= icons[i].x_press + icons[i].width &&
        y >= icons[i].y_press && y <= icons[i].y_press + icons[i].height) {
            is_hover = 1;
			icon_hover = i;
            XDefineCursor(display, window, hand_cursor);
            break;
        }
    }

	if (is_hover == 1 && icon_hover != -1 && already_hover == 0) {
		icons[icon_hover].x -= 5;
        icons[icon_hover].y -= 5;
        icons[icon_hover].width += 10;
        icons[icon_hover].height += 10;
        icons[icon_hover].pixmap = -1;
		already_hover = 1;
		XEvent event = { .type = Expose };
        XSendEvent(display, window, 0, ExposureMask, &event);
	} else if (is_hover == 0 && icon_hover != -1) {
		icons[icon_hover].x += 5;
        icons[icon_hover].y += 5;
        icons[icon_hover].width -= 10;
        icons[icon_hover].height -= 10;
		icons[icon_hover].pixmap = -1;
		icon_hover = -1;
		already_hover = 0;
		XEvent event = { .type = Expose };
        XSendEvent(display, window, 0, ExposureMask, &event);
		XDefineCursor(display, window, cursor);
	}

    return is_hover;
}

void icons_on_press(int x, int y) {
    for (int i = 0; i < app_shortcuts_length; i++) {
        if (x >= icons[i].x_press && x <= icons[i].x_press + icons[i].width &&
        y >= icons[i].y_press && y <= icons[i].y_press + icons[i].height) {
            if (fork() == 0) {
                execlp(icons[i].exec, icons[i].exec, NULL);
            }
			title_launched = malloc(sizeof(char) * (strlen(icons[i].name) + 1));
			strcpy(title_launched, icons[i].name);
        }
    }
}

void icon_scroll_up(void) {
	if (app_shortcuts_length <= 4) {
		return;
	}

	if (icons[0].y - 20 > up_line_y) {
		return;
	}

	for (int i = 0; i < app_shortcuts_length; i++) {
		icons[i].y += 20;
		icons[i].y_press += 20;
	}

	icon_hover = -1;
}

void icon_scroll_down(void) {
	if (app_shortcuts_length <= 4) {
		return;
	}

	if (icons[app_shortcuts_length - 1].y + icons[app_shortcuts_length - 1].height + 20 < bottom_line_y) {
		return;
	}

	for (int i = 0; i < app_shortcuts_length; i++) {
		icons[i].y -= 20;
		icons[i].y_press -= 20;
	}

	icon_hover = -1;
}