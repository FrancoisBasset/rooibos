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
	XImage *image;
} icon_t;

void icon_draw(icon_t icon);
cairo_surface_t* icon_get_surface_png(const char *filename);
cairo_surface_t* icon_get_surface_jpg(const char *filename);
XImage* icon_get_image_xpm(const char *filename);
RsvgHandle* icon_get_surface_svg(const char *filename);
void icon_draw_png(cairo_surface_t *cairo_surface, const char* name, int x, int y, int width, int height);
void icon_draw_svg(RsvgHandle *rsvg_handle, const char* name, int x, int y, int width, int height);
void icon_draw_xpm(XImage *image, const char *name, int x, int y, int width, int height);
void icons_init(void);
void icons_show(void);
int icons_on_hover(int x, int y);
void icons_on_press(int x, int y);
void icon_scroll_up(void);
void icon_scroll_down(void);
#endif
