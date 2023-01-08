#ifndef CACHE_H
#define CACHE_H
#include "appshortcut.h"
int cache_init(void);
int cache_update(void);
appshortcut_t* cache_get_app_shortcuts(int *length);
#endif
