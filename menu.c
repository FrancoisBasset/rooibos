#include <stdlib.h>
#include <X11/Xlib.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <math.h>
#include "objects.h"
#include "menu.h"
#include "icon.h"

menu_t menu;

void menu_init(void) {
    int width = (int) (screen_width * 0.70);
    int height = (int) (screen_height * 0.70);
    int x = (screen_width - width) / 2;
    int y = (screen_height - height) / 2;

    menu_t tmp_menu = { x, y, width, height, 0, 0 };
    menu = tmp_menu;
}

void menu_show(void) {
    cairo_surface_t *x11_surface = cairo_xlib_surface_create(display, window, XDefaultVisualOfScreen(screen), screen_width, screen_height);
    cairo_t *context = cairo_create(x11_surface);

    cairo_set_source_rgba(context, 0.85, 0.85, 0.85, 0.7);

    cairo_move_to(context, menu.x + 50, menu.y);

    cairo_line_to(context, menu.x + menu.width - 50, menu.y);
    cairo_arc(context, menu.x + menu.width - 50, menu.y + 50, 50, 270 * (M_PI / 180), 360 * (M_PI / 180));

    cairo_line_to(context, menu.x + menu.width, menu.y + menu.height - 50);
    cairo_arc(context, menu.x + menu.width - 50, menu.y + menu.height - 50, 50, 0 * (M_PI / 180), 90 * (M_PI / 180));

    cairo_line_to(context, menu.x + 50, menu.y + menu.height);
    cairo_arc(context, menu.x + 50, menu.y + menu.height - 50, 50, 90 * (M_PI / 180), 180 * (M_PI / 180));

    cairo_line_to(context, menu.x, menu.y + 50);
    cairo_arc(context, menu.x + 50, menu.y + 50, 50, 180 * (M_PI / 180), 270 * (M_PI / 180));

    cairo_stroke_preserve(context);
    cairo_fill(context);

    menu.is_showed = 1;

    icons_show();
}

void menu_clear(void) {
    XClearArea(display, window, menu.x - 1, menu.y - 1, menu.width + 2, menu.height + 2, 1);
    menu.is_showed = 0;
}