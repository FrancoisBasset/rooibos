#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "appshortcut.h"
#include "cache.h"
#include "utils.h"
#include "VERSION.h"

void green(void) {
	printf("\033[0;32m");
}

void red(void) {
	printf("\033[0;31m");
}

void yellow(void) {
	printf("\033[0;33m");
}

void reset(void) {
	printf("\033[0m");
}

int main(int argc, char **argv) {
	if (argc > 1) {
		if (strcmp(argv[1], "cache-init") == 0) {
			yellow();
			printf("rooibos : Starting cache init...\n");
			reset();

			int init_status = cache_init();
			if (init_status == 0) {
				green();
				printf("rooibos : Success cache init ! :)\n");
				reset();
			} else {
				red();
				printf("rooibos : Error cache init ! :(\n");
				reset();
				return -1;
			}
		} else if (strcmp(argv[1], "cache-update") == 0) {
			yellow();
			printf("rooibos : Starting cache update...\n");
			reset();

			int update_status = cache_update();
			if (update_status == 0) {
				green();
				printf("rooibos : Success cache update ! :)\n");
				reset();
			} else {
				red();
				printf("rooibos : Error cache update ! :(\n");
				reset();
				return -1;
			}
		} else if (strcmp(argv[1], "version") == 0) {
			printf("%s\n", VERSION);
		} else {
			red();
			printf("Not a valid command !\n");
			reset();
			return -1;
		}

		return 0;
	}

	int is_root = utils_is_root();
	if (is_root) {
		red();
		printf("Root is not allowed to launch the rooibos window manager\n");
		reset();
		return -1;
	}

	int length = 0;
	struct appshortcut *app_shortcuts = cache_get_app_shortcuts(&length);
	if (app_shortcuts == NULL) {
		red();
		printf("rooibos : Error cache get");
		reset();
		return -1;
	}

	char *categories[9] = { "Games", "System", "Graphics", "Development", "Network", "Multimedia", "Office", "Settings", "Other" };
	int new_length = 0;

	for (int i = 0; i < 9; i++) {
		green();
		printf("%s \n", categories[i]);
		reset();

		struct appshortcut *app_shortcut= appshortcut_get_app_shortcuts_by_category(app_shortcuts, categories[i], length, &new_length);

		for (int j = 0; j < new_length; j++) {
			printf("%s\n%s -- %s -- %s -- %s\n\n", app_shortcut[j].file, app_shortcut[j].name, app_shortcut[j].exec, app_shortcut[j].category, app_shortcut[j].icon);
		}

		if (new_length == 0) {
			red();
			printf("0 apps\n\n");
			reset();
		}

		free(app_shortcut);
	}

	for (int i = 0; i < length; i++) {
		free(app_shortcuts[i].name);
		free(app_shortcuts[i].exec);
		free(app_shortcuts[i].category);
		free(app_shortcuts[i].icon);
		free(app_shortcuts[i].file);
	}

	free(app_shortcuts);
}
