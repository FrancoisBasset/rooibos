#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "main.h"
#include "appshortcut.h"
#include "cache.h"
#include "utils.h"
#include "VERSION.h"
#include "rooibos.h"

#define green_color "\033[0;32m"
#define red_color "\033[0;31m"
#define yellow_color "\033[0;33m"
#define normal_color "\033[0m"

int main(int argc, char **argv) {
	if (argc > 1) {
		return handle_arguments(argv);
	}

	int is_root = utils_is_root();
	if (is_root) {
		printf("%sRoot is not allowed to launch the rooibos window manager%s\n", red_color, normal_color);
		return -1;
	}

	int length = 0;
	struct appshortcut_t *app_shortcuts = cache_get_app_shortcuts(&length);

	show_apps_by_category(app_shortcuts, length);

	for (int i = 0; i < length; i++) {
		free(app_shortcuts[i].name);
		free(app_shortcuts[i].exec);
		free(app_shortcuts[i].category);
		free(app_shortcuts[i].icon);
		free(app_shortcuts[i].file);
	}

	free(app_shortcuts);

	rooibos();
}

void show_apps_by_category(const struct appshortcut_t *app_shortcuts, int length) {
	char *categories[9] = { "Games", "System", "Graphics", "Development", "Network", "Multimedia", "Office", "Settings", "Other" };
	int new_length = 0;

	for (int i = 0; i < 9; i++) {
		printf("%s%s%s \n", green_color, categories[i], normal_color);

		struct appshortcut_t *shortcuts_categories = appshortcut_get_app_shortcuts_by_category(app_shortcuts, categories[i], length, &new_length);

		for (int j = 0; j < new_length; j++) {
			printf("%s\n%s -- %s -- %s -- %s\n\n", shortcuts_categories[j].file, shortcuts_categories[j].name, shortcuts_categories[j].exec, shortcuts_categories[j].category, shortcuts_categories[j].icon);
		}

		if (new_length == 0) {
			printf("%s0 apps%s\n\n", red_color, normal_color);
		}

		free(shortcuts_categories);
	}
}

int handle_arguments(char **argv) {
	if (strcmp(argv[1], "cache:init") == 0) {
		printf("%srooibos : Starting cache init...%s\n", yellow_color, normal_color);

		int init_status = cache_init();
		if (init_status == 0) {
			printf("%srooibos : Success cache init ! :)%s\n", green_color, normal_color);
		} else {
			printf("%srooibos : Error cache init ! :(%s\n", red_color, normal_color);
			return -1;
		}
	} else if (strcmp(argv[1], "cache:update") == 0) {
		printf("%srooibos : Starting cache update...%s\n", yellow_color, normal_color);

		int update_status = cache_update();
		if (update_status == 0) {
			printf("%srooibos : Success cache update ! :)%s\n", green_color, normal_color);
		} else {
			printf("%srooibos : Error cache update ! :(%s\n", red_color, normal_color);
			return -1;
		}
	} else if (strcmp(argv[1], "version") == 0) {
		printf("%s\n", VERSION);
	} else {
		printf("%sNot a valid command !%s\n", red_color, normal_color);
		return -1;
	}

	return 0;
}
