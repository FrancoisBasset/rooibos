#ifndef APPSHORTCUT_H
#define APPSHORTCUT_H
#include <sys/stat.h>

struct appshortcut {
	char *name;
	char *exec;
	char *category;
	char *icon;
	char *file;
};

char** appshortcut_get_all_desktop_files(int *length);
char** appshortcut_get_desktop_files(char *foldername, int *length);
struct appshortcut appshortcut_get_app_shortcut(char *filename);
struct appshortcut* appshortcut_get_app_shortcuts_by_category(struct appshortcut* app_shortcuts, char *category, int length, int *new_length);

static int filter_icon(const char *path, const struct stat *sb, int typeflag);
static char* appshortcut_get_category(char* categories);
static char* appshortcut_get_icon(char *icon);
#endif
