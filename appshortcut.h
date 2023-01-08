#ifndef APPSHORTCUT_H
#define APPSHORTCUT_H
#include <sys/stat.h>

typedef struct appshortcut_t {
	char *name;
	char *exec;
	char *category;
	char *icon;
	char *file;
} appshortcut_t;

char** appshortcut_get_all_desktop_files(int *length);
char** appshortcut_get_desktop_files(const char *foldername, int *length);
appshortcut_t appshortcut_get_app_shortcut(const char *filename);
appshortcut_t* appshortcut_get_app_shortcuts_by_category(const appshortcut_t* app_shortcuts, const char *category, int length, int *new_length);
#endif
