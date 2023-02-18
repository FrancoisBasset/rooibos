#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include "utils.h"

struct passwd *pw = NULL;

static char* utils_get_wallpaper_to_use();

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
		case UTILS_WALLPAPER_PNG:
			strcat(result, "/.rooibos/wallpaper.png");
			break;
		case UTILS_WALLPAPER_JPG:
			strcat(result, "/.rooibos/wallpaper.jpg");
			break;
		case UTILS_WALLPAPER_TO_USE:
			result = utils_get_wallpaper_to_use();
			break;
	}

	return result;
}

int utils_is_root(void) {
	return getuid() == 0 ? 1 : 0;
}

static char* utils_get_wallpaper_to_use() {
	char *wallpaper_file_path = malloc(sizeof(char) * 200);
	strcpy(wallpaper_file_path, "/usr/share/rooibos/wallpaper.jpg");

	char *jpg = utils_get(UTILS_WALLPAPER_JPG);
	if (access(jpg, F_OK) == 0) {
		strcpy(wallpaper_file_path, jpg);
	}
	free(jpg);

	char *png = utils_get(UTILS_WALLPAPER_PNG);
	if (access(png, F_OK) == 0) {
		strcpy(wallpaper_file_path, png);
	}
	free(png);

	if (access("./assets/wallpaper.jpg", F_OK) == 0) {
		strcpy(wallpaper_file_path, "./assets/wallpaper.jpg");
	}
	if (access("./assets/wallpaper.png", F_OK) == 0) {
		strcpy(wallpaper_file_path, "./assets/wallpaper.png");
	}

	return wallpaper_file_path;
}
