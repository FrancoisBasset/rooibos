#ifndef ICON_H
#define ICON_H
#include <cairo/cairo.h>
#include <cairo/cairo-xlib.h>
#include <librsvg-2.0/librsvg/rsvg.h>

typedef struct icon_t {
    char *filename;
    char* name;
    char* exec;
    int x;
    int y;
    int width;
    int height;
    cairo_surface_t *cairo_surface;
    RsvgHandle *rsvg_handle;
} icon_t;

void icon_draw(icon_t icon);
cairo_surface_t* icon_get_surface_png(const char *filename);
RsvgHandle* icon_get_surface_svg(const char *filename);
void icon_draw_png(cairo_surface_t *cairo_surface, const char* name, int x, int y, int width, int height);
void icon_draw_svg(RsvgHandle *rsvg_handle, const char* name, int x, int y, int width, int height);
void icons_init(void);
void icons_show(void);
void icons_clear(void);
void icons_on_hover(int x, int y);
void icons_on_click(int x, int y);
#endif
