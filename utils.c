#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include "utils.h"

struct passwd *pw = NULL;

char* utils_get(utils_t util) {
	if (pw == NULL) {
		pw = getpwnam(getlogin());
	}

	char *result = malloc(sizeof(char) * 200);
	strcpy(result, pw->pw_dir);

	switch (util) {
		case UTILS_LOGIN:
			return pw->pw_name;
		case UTILS_USER_HOME:
			return pw->pw_dir;
		case UTILS_LOCAL_APPS:
			strcat(result, "/.local/share/applications/");
			break;
		case UTILS_LOCAL_ICONS:
			strcat(result, "/.local/share/icons/");
			break;
		case UTILS_FOLDER:
			strcat(result, "/.rooibos");
			break;
		case UTILS_CACHE:
			strcat(result, "/.rooibos/cache.db");
			break;
		case UTILS_LOGO:
			strcat(result, "/.rooibos/logo.svg");
			break;
		case UTILS_WALLPAPER:
			strcat(result, "/.rooibos/wallpaper.png");
			break;
	}

	return result;
}

int utils_is_root(void) {
	return getuid() == 0 ? 1 : 0;
}
