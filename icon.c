#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <librsvg-2.0/librsvg/rsvg.h>
#include "objects.h"
#include "icon.h"

void icon_draw_png(char *filename, int x, int y, int width, int height) {
    cairo_surface_t *x11_surface = cairo_xlib_surface_create(display, window, XDefaultVisualOfScreen(screen), screen_width - 100, screen_height - 100);

    cairo_surface_t *sur = cairo_image_surface_create_from_png(filename);
    cairo_t *context = cairo_create(x11_surface);

    double surface_width = (double) cairo_image_surface_get_width(sur) / width;
    double surface_height = (double) cairo_image_surface_get_height(sur) / height;

    cairo_surface_set_device_scale(sur, surface_width, surface_height);

    cairo_set_source_surface(context, sur, x, y);
    cairo_paint(context);

    cairo_surface_destroy(x11_surface);
    cairo_surface_destroy(sur);
}

void icon_draw_svg(char *filename, int x, int  y, int width, int height) {
    RsvgHandle *handle = rsvg_handle_new_from_file(filename, NULL);
    RsvgRectangle rectangle = { .x = x, .y = y, .width = width, .height = height };

    cairo_surface_t *x11_surface = cairo_xlib_surface_create(display, window, XDefaultVisualOfScreen(screen), screen_width, screen_height);
    cairo_t *context = cairo_create(x11_surface);

    rsvg_handle_render_document(handle, context, &rectangle, NULL);
}
