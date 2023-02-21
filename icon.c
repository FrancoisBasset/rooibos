#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <librsvg-2.0/librsvg/rsvg.h>
#include <math.h>
#include "objects.h"
#include "icon.h"
#include "rooibos.h"
#include "appshortcut.h"
#include "cache.h"
#include "menu.h"
#include "cairo_jpg.h"

icon_t *icons;
int app_shortcuts_length = 0;
char *icon_hover = NULL;
int previous_icon = -1;

int icon_width = 0;

void icon_draw(icon_t icon) {
    if (strstr(icon.filename, ".svg") != NULL) {
        icon_draw_svg(icon.rsvg_handle, icon.name, icon.x, icon.y, icon.width, icon.height);
    } else if (strstr(icon.filename, ".png") != NULL) {
        icon_draw_png(icon.cairo_surface, icon.name, icon.x, icon.y, icon.width, icon.width);
    }
}

cairo_surface_t* icon_get_surface_png(const char *filename) {
    cairo_surface_t *cairo_surface = cairo_image_surface_create_from_png(filename);
    return cairo_surface;
}

cairo_surface_t* icon_get_surface_jpg(const char *filename) {
    cairo_surface_t *cairo_surface = cairo_image_surface_create_from_jpeg(filename);
    return cairo_surface;
}

RsvgHandle* icon_get_surface_svg(const char *filename) {
    RsvgHandle *handle = rsvg_handle_new_from_file(filename, NULL);
    return handle;
}

void icon_draw_png(cairo_surface_t *cairo_surface, const char* name, int x, int y, int width, int height) {
    cairo_surface_t *x11_surface = cairo_xlib_surface_create(display, window, XDefaultVisualOfScreen(screen), screen_width, screen_height);
    cairo_t *context = cairo_create(x11_surface);

    double surface_width = (double) cairo_image_surface_get_width(cairo_surface) / width;
    double surface_height = (double) cairo_image_surface_get_height(cairo_surface) / height;

    cairo_surface_set_device_scale(cairo_surface, surface_width, surface_height);

    cairo_set_source_surface(context, cairo_surface, x, y);
    cairo_paint(context);

    int name_width = XTextWidth(font_struct, name, (int) strlen(name));
    int name_length = (int) strlen(name);

    while (name_width > width) {
        name_length--;
        name_width = XTextWidth(font_struct, name, name_length);
    }

    XDrawString(display, window, gc_text_white, x, y + width + 10, name, name_length);

    cairo_surface_destroy(x11_surface);
}

void icon_draw_svg(RsvgHandle *rsvg_handle, const char* name, int x, int  y, int width, int height) {
    RsvgRectangle rectangle = { .x = x, .y = y, .width = width, .height = height };

    cairo_surface_t *x11_surface = cairo_xlib_surface_create(display, window, XDefaultVisualOfScreen(screen), screen_width, screen_height);
    cairo_t *context = cairo_create(x11_surface);

    int name_width = XTextWidth(font_struct, name, (int) strlen(name));
    int name_length = (int) strlen(name);
    
    while (name_width > width) {
        name_length--;
        name_width = XTextWidth(font_struct, name, name_length);
    }

    XDrawString(display, window, gc_text_white, x, y + width + 10, name, name_length);

    rsvg_handle_render_document(rsvg_handle, context, &rectangle, NULL);
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
    
    int x = menu.x;
    int y = menu.y + 100;

    icons = malloc(sizeof(icon_t) * app_shortcuts_length);

    int square_root = (int) sqrt(app_shortcuts_length);
    int nb_x = square_root + 1;
    int nb_y = square_root + 1;

    while (1) {
        if (menu.width > menu.height) {
            if ((nb_x + 1) * (nb_y - 1) < app_shortcuts_length) {
                break;
            }

            nb_x++;
            nb_y--;
        } else {
            if ((nb_x - 1) * (nb_y + 1) < app_shortcuts_length) {
                break;
            }

            nb_x--;
            nb_y++;
        }
    }

    const int menu_width = (int) (menu.width * 0.60);
    const int menu_height = (int) (menu.height * 0.90);

    const int space_x = (menu.width - menu_width) / nb_x;
    const int space_y = (menu.height - menu_height) / nb_y;

    x += (space_x / 2);
    y += (space_y / 2);

    const int width = menu_width / nb_x;
    const int height = menu_height / nb_y;

    icon_width = width < height ? width : height;

    for (int i = 0; i < app_shortcuts_length; i++) {
        if (x >= menu.x + menu.width - icon_width) {
            x = menu.x + (space_x / 2);
            y += icon_width + space_y + (space_y / 2);
        }

        icon_t new_icon = { app_shortcuts[i].icon, app_shortcuts[i].name, app_shortcuts[i].exec, x, y, icon_width, icon_width, NULL, NULL };

        if (strstr(app_shortcuts[i].icon, ".svg") != NULL) {
            new_icon.rsvg_handle = icon_get_surface_svg(new_icon.filename);
        } else if (strstr(app_shortcuts[i].icon, ".png") != NULL) {
            new_icon.cairo_surface = icon_get_surface_png(new_icon.filename);
        }

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

void icons_show(void) {
    for (int i = 0; i < app_shortcuts_length; i++) {
        icon_draw(icons[i]);
    }
}

int icons_on_hover(int x, int y) {
    int is_hover = 0;

    for (int i = 0; i < app_shortcuts_length; i++) {
        if (x >= icons[i].x && x <= icons[i].x + icons[i].width &&
        y >= icons[i].y && y <= icons[i].y + icons[i].height) {
            is_hover = 1;
            
            if (icon_hover != NULL && strcmp(icon_hover, icons[i].name) == 0) {
                previous_icon = i;
                break;
            }

            XDefineCursor(display, window, hand_cursor);
            
            if (previous_icon != -1) {
                icons[previous_icon].x += 5;
                icons[previous_icon].y += 5;
                icons[previous_icon].width -= 10;
                icons[previous_icon].height -= 10;
            }

            icons[i].x -= 5;
            icons[i].y -= 5;
            icons[i].width += 10;
            icons[i].height += 10;

            icon_hover = icons[i].name;
            previous_icon = i;

            //XEvent event = { .type = Expose };
            //XSendEvent(display, window, 0, ExposureMask, &event);
            
            break;
        }
    }

    if (icon_hover != NULL && is_hover == 0) {
        icon_hover = NULL;
        //XEvent event = { .type = Expose };
        //XSendEvent(display, window, 0, ExposureMask, &event);
        XDefineCursor(display, window, cursor);
    }

    return is_hover;
}

void icons_on_press(int x, int y) {
    for (int i = 0; i < app_shortcuts_length; i++) {
        if (x >= icons[i].x && x <= icons[i].x + icons[i].width &&
        y >= icons[i].y && y <= icons[i].y + icons[i].height) {
            if (fork() == 0) {
                execlp(icons[i].exec, icons[i].exec, NULL);
            }
        }
    }
}
