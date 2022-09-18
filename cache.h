#ifndef CACHE_H
#define CACHE_H
int cache_init(void);
int cache_update(void);
struct appshortcut* cache_get_app_shortcuts(int *length);
#endif
