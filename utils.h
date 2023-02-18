#ifndef UTILS_H
#define UTILS_H

typedef enum utils_t {
    UTILS_LOGIN,
    UTILS_USER_HOME,
    UTILS_LOCAL_APPS,
    UTILS_LOCAL_ICONS,
	UTILS_FOLDER,
    UTILS_CACHE,
    UTILS_LOGO,
    UTILS_WALLPAPER_PNG,
    UTILS_WALLPAPER_JPG,
	UTILS_WALLPAPER_TO_USE
} utils_t;

char* utils_get(utils_t util);
int utils_is_root(void);
#endif
