#include <stdlib.h>
#include <X11/Xlib.h>
#include <cairo.h>
#include <cairo-xlib.h>
#include <math.h>
#include "objects.h"
#include "menu.h"
#include "icon.h"

menu_t menu;
char *categories[9] = { "Games", "System", "Graphics", "Development", "Network", "Multimedia", "Office", "Settings", "Other" };
category_button_t left_category_button;
category_button_t right_category_button;

void menu_init(void) {
    int width = (int) (screen_width * 0.70);
    int height = (int) (screen_height * 0.70);
    int x = (screen_width - width) / 2;
    int y = (screen_height - height) / 2;

    menu_t tmp_menu = { x, y, width, height, -1, 0 };
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

    char *category_name = malloc(sizeof(char) * 20);
    if (menu.category_index == -1) {
        strcpy(category_name, "All categories");
    } else {
        strcpy(category_name, categories[menu.category_index]);
    }

    left_category_button.visible = 0;
    right_category_button.visible = 0;
    int width;
    int height = 20;

    width = XTextWidth(font_struct, category_name, (int) strlen(category_name));
    int x = (screen_width - width) / 2;
    XDrawString(display, window, gc_text_black, x, menu.y + 50, category_name, (int) strlen(category_name));

    if (menu.category_index == 0) {
        width = XTextWidth(font_struct, "< All categories", 16);
        left_category_button = (category_button_t) { .visible = 1, .x = menu.x + 40, menu.y + 25, .width = width + 20, .height = height + 20 };

        XFillRectangle(display, window, gc_category_button, left_category_button.x, left_category_button.y, left_category_button.width, left_category_button.height);
        XDrawString(display, window, gc_text_white, menu.x + 50, menu.y + 50, "< All categories", 16);
    }
    
    if (menu.category_index > 0) {
        strcpy(category_name, "< ");
        strcat(category_name, categories[menu.category_index - 1]);

        width = XTextWidth(font_struct, category_name, (int) strlen(category_name));
        left_category_button = (category_button_t) { .visible = 1, .x = menu.x + 40, menu.y + 25, .width = width + 20, .height = height + 20};

        XFillRectangle(display, window, gc_category_button, left_category_button.x, left_category_button.y, left_category_button.width, left_category_button.height);
        XDrawString(display, window, gc_text_white, menu.x + 50, menu.y + 50, category_name, (int) strlen(category_name));
    }
    
    if (menu.category_index < 8) {
        strcpy(category_name, categories[menu.category_index + 1]);
        strcat(category_name, " >");

        width = XTextWidth(font_struct, category_name, (int) strlen(category_name));
        right_category_button = (category_button_t) { .visible = 1, .x = menu.x + menu.width - width - 60, menu.y + 25, .width = width + 20, .height = height + 20 };

        XFillRectangle(display, window, gc_category_button, right_category_button.x, right_category_button.y, right_category_button.width, right_category_button.height);
        XDrawString(display, window, gc_text_white, menu.x + menu.width - width - 50, menu.y + 50, category_name, (int) strlen(category_name));
    }

    free(category_name);
}

void menu_clear(void) {
    XClearArea(display, window, menu.x - 1, menu.y - 1, menu.width + 2, menu.height + 2, 1);
    menu.is_showed = 0;
}

int menu_category_buttons_on_hover(int x, int y) {
    int left_is_hover = left_category_button.visible == 1 &&
        x >= left_category_button.x && x <= left_category_button.x + left_category_button.width &&
        y >= left_category_button.y && y <= left_category_button.y + left_category_button.height;

    int right_is_hover = right_category_button.visible == 1 &&
        x >= right_category_button.x && x <= right_category_button.x + right_category_button.width &&
        y >= right_category_button.y && y <= right_category_button.y + right_category_button.height;

    if (left_is_hover == 1) {
        XDefineCursor(display, window, hand_cursor);
        return 1;
    } else if (right_is_hover == 1) {
        XDefineCursor(display, window, hand_cursor);
        return 2;
    }
    
    XDefineCursor(display, window, cursor);
    return 0;
}

void menu_category_buttons_on_press(int category_button) {
    if (category_button == 0) {
        return;
    } else if (category_button == 1) {
        menu_go_to_previous_category();
    } else if (category_button == 2) {
        menu_go_to_next_category();
    }
}

void menu_go_to_previous_category(void) {
    menu.category_index--;
    icons_init();
    menu_clear();
    menu_show();
}

void menu_go_to_next_category(void) {
    menu.category_index++;
    icons_init();
    menu_clear();
    menu_show();
}