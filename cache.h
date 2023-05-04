#ifndef CACHE_H
#define CACHE_H
#include <sqlite3.h>
#include "appshortcut.h"
int cache_init(void);
int cache_update(void);
appshortcut_t* cache_get_app_shortcuts(int *length);
int cache_init_table(sqlite3 *db, const char *sql);
char** cache_get_history(int *length);
void cache_add_history(const char *command);
int cache_exec_exists(const char *exec, const char* file);
#endif
