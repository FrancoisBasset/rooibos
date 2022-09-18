#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "appshortcut.h"
#include "cache.h"

int main(int argc, char **argv) {
	if (argc > 1) {
		if (strcmp(argv[1], "cache-init") == 0) {
			printf("Beginning cache init\n");

			int init_status = cache_init();
			if (init_status == 0) {
				printf("Success cache init !\n");
			} else {
				printf("Error cache init !\n");
				return -1;
			}
		} else if (strcmp(argv[1], "cache-update") == 0) {
			printf("Beginning cache update !\n");

			int update_status = cache_update();
			if (update_status == 0) {
				printf("Success cache update !\n");
			} else {
				printf("Error cache update !\n");
				return -1;
			}
		} else {
			printf("Not a valid command !\n");
			return -1;
		}

		return 0;
	}

	int length = 0;
	struct appshortcut *app_shortcuts = cache_get_app_shortcuts(&length);

	for (int i = 0; i < length; i++) {
		printf("%s\n%s -- %s -- %s -- %s\n\n", app_shortcuts[i].file, app_shortcuts[i].name, app_shortcuts[i].exec, app_shortcuts[i].categorie, app_shortcuts[i].icon);
		free(app_shortcuts[i].name);
		free(app_shortcuts[i].exec);
		free(app_shortcuts[i].categorie);
		free(app_shortcuts[i].icon);
		free(app_shortcuts[i].file);
	}

	free(app_shortcuts);
}