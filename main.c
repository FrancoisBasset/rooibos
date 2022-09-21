#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "appshortcut.h"
#include "cache.h"

int main(int argc, char **argv) {
	if (argc > 1) {
		if (strcmp(argv[1], "cache-init") == 0) {
			printf("rooibos : Starting cache init...\n");

			int init_status = cache_init();
			if (init_status == 0) {
				printf("rooibos : Success cache init ! :)\n");
			} else {
				printf("rooibos : Error cache init ! :(\n");
				return -1;
			}
		} else if (strcmp(argv[1], "cache-update") == 0) {
			printf("rooibos : Starting cache update...\n");

			int update_status = cache_update();
			if (update_status == 0) {
				printf("rooibos : Success cache update ! :)\n");
			} else {
				printf("rooibos : Error cache update ! :(\n");
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
	if (app_shortcuts == NULL) {
		printf("rooibos : Error cache get");
		return -1;
	}

	for (int i = 0; i < length; i++) {
		printf("%s\n%s -- %s -- %s -- %s\n\n", app_shortcuts[i].file, app_shortcuts[i].name, app_shortcuts[i].exec, app_shortcuts[i].category, app_shortcuts[i].icon);
		free(app_shortcuts[i].name);
		free(app_shortcuts[i].exec);
		free(app_shortcuts[i].category);
		free(app_shortcuts[i].icon);
		free(app_shortcuts[i].file);
	}

	free(app_shortcuts);
}
