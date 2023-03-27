#ifndef UTILS_H
#define UTILS_H

typedef enum utils_t {
    UTILS_LOGIN,
    UTILS_USER_HOME,
    UTILS_LOCAL_APPS,
    UTILS_LOCAL_ICONS,
	UTILS_FOLDER,
    UTILS_CACHE,
	UTILS_LOGO_TO_USE,
	UTILS_WALLPAPER_TO_USE,
	UTILS_NEW_TERMINAL,
	UTILS_LOGOUT
} utils_t;

char* utils_get(utils_t util);
int utils_is_root(void);
#endif
