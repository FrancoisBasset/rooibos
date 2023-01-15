#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <librsvg-2.0/librsvg/rsvg.h>
#include "objects.h"
#include "icon.h"
#include "rooibos.h"
#include "appshortcut.h"
#include "cache.h"

icon_t *icons;
int app_shortcuts_length = 0;
int menu_height = 0;
char *icon_hover = NULL;
int previous_icon;

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

RsvgHandle* icon_get_surface_svg(const char *filename) {
    RsvgHandle *handle = rsvg_handle_new_from_file(filename, NULL);
    return handle;
}

void icon_draw_png(cairo_surface_t *cairo_surface, const char* name, int x, int y, int width, int height) {
    cairo_surface_t *x11_surface = cairo_xlib_surface_create(display, window, XDefaultVisualOfScreen(screen), screen_width - 100, screen_height - 100);
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
    appshortcut_t *app_shortcuts = cache_get_app_shortcuts(&app_shortcuts_length);
    int x = 0;
    int y = 0;
    int width = 100;

    icons = malloc(sizeof(icon_t) * app_shortcuts_length);

    for (int i = 0; i < app_shortcuts_length; i++) {
        if (x >= screen_width - width - 20) {
            x = 0;
            y += width + 40;
            menu_height = y + width + 20;
        }

        icon_t new_icon = { app_shortcuts[i].icon, app_shortcuts[i].name, app_shortcuts[i].exec, x, y, width, width, NULL, NULL };

        if (strstr(app_shortcuts[i].icon, ".svg") != NULL) {
            new_icon.rsvg_handle = icon_get_surface_svg(new_icon.filename);
        } else if (strstr(app_shortcuts[i].icon, ".png") != NULL) {
            new_icon.cairo_surface = icon_get_surface_png(new_icon.filename);
        }

        icons[i] = new_icon;

        x += width;
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

void icons_clear(void) {
    XClearArea(display, window, 0, 0, screen_width, menu_height, 1);
}

void icons_on_hover(int x, int y) {
    XColor color_taskbar = {
        .red = 15000,
        .green = 15000,
        .blue = 55000
    };
    XAllocColor(display, colormap, &color_taskbar);
    XGCValues gcv = { .foreground = color_taskbar.pixel };
    GC gc = XCreateGC(display, window, GCForeground, &gcv);

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
            XClearArea(display, window, icons[previous_icon].x, icons[previous_icon].y, 100, 100, 1);
            XFillRectangle(display, window, gc, icons[i].x, icons[i].y, icons[i].width, icons[i].height);
            
            icon_draw(icons[previous_icon]);
            icon_draw(icons[i]);

            icon_hover = icons[i].name;
            previous_icon = i;
            
            break;
        }
    }

    if (icon_hover != NULL && is_hover == 0) {
        icon_hover = NULL;
        XClearArea(display, window, icons[previous_icon].x, icons[previous_icon].y, 100, 100, 1);
        icon_draw(icons[previous_icon]);
        XDefineCursor(display, window, cursor);
    }
}

void icons_on_click(int x, int y) {
    for (int i = 0; i < app_shortcuts_length; i++) {
        if (x >= icons[i].x && x <= icons[i].x + icons[i].width &&
        y >= icons[i].y && y <= icons[i].y + icons[i].height) {
            if (fork() == 0) {
                execlp(icons[i].exec, icons[i].exec, NULL);
            }
        }
    }
}
