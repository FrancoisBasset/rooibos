#ifndef MAIN_H
#define MAIN_H
#include "appshortcut.h"
void show_apps_by_category(struct appshortcut *app_shortcuts, int length);
int handle_arguments(char **argv);
#endif
