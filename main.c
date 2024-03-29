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

	rooibos();
}

int handle_arguments(char **argv) {
	if (strcmp(argv[1], "cache-init") == 0) {
		printf("%srooibos : Starting cache init...%s\n", yellow_color, normal_color);

		int init_status = cache_init();
		if (init_status == 0) {
			printf("%srooibos : Success cache init ! :)%s\n", green_color, normal_color);
		} else {
			printf("%srooibos : Error cache init ! :(%s\n", red_color, normal_color);
			return -1;
		}
	} else if (strcmp(argv[1], "cache-update") == 0) {
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
