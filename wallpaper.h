#ifndef WALLPAPER_H
#define WALLPAPER_H
#include <X11/Xlib.h>
extern Window wallpaper_window;
char* wallpaper_get_current(void);
void wallpaper_show(int splash);
void wallpaper_open_window(void);
void wallpaper_update_thumbnail(void);
void wallpaper_expose(void);
void wallpaper_on_close(void);
void wallpaper_on_press(int x, int y);
void wallpaper_on_key_press(XKeyEvent key_event);
#endif