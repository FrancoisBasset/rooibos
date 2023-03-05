#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include "utils.h"

struct passwd *pw = NULL;

static char* utils_get_wallpaper_to_use();
static char* utils_get_logo_to_use();
static char* utils_get_assets(const char *filename);

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
		case UTILS_LOGO_TO_USE:
			result = utils_get_logo_to_use();
			break;
		case UTILS_LOGO_SVG:
			strcat(result, "/.rooibos/logo.svg");
			break;
		case UTILS_LOGO_JPG:
			strcat(result, "/.rooibos/logo.jpg");
			break;
		case UTILS_LOGO_PNG:
			strcat(result, "/.rooibos/logo.png");
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
		case UTILS_NEW_TERMINAL:
			result = utils_get_assets("new_terminal.png");
			break;
		case UTILS_LOGOUT:
			result = utils_get_assets("logout.png");
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

static char* utils_get_logo_to_use() {
	char *logo_file_path = malloc(sizeof(char) * 200);
	strcpy(logo_file_path, "/usr/share/rooibos/logo.svg");

	char *svg = utils_get(UTILS_LOGO_SVG);
	if (access(svg, F_OK) == 0) {
		strcpy(logo_file_path, svg);
	}
	free(svg);

	char *jpg = utils_get(UTILS_LOGO_JPG);
	if (access(jpg, F_OK) == 0) {
		strcpy(logo_file_path, jpg);
	}
	free(jpg);

	char *png = utils_get(UTILS_LOGO_PNG);
	if (access(png, F_OK) == 0) {
		strcpy(logo_file_path, png);
	}
	free(png);

	if (access("./assets/logo.svg", F_OK) == 0) {
		strcpy(logo_file_path, "./assets/logo.svg");
	}
	if (access("./assets/logo.jpg", F_OK) == 0) {
		strcpy(logo_file_path, "./assets/logo.jpg");
	}
	if (access("./assets/logo.png", F_OK) == 0) {
		strcpy(logo_file_path, "./assets/logo.png");
	}

	return logo_file_path;
}

static char* utils_get_assets(const char *filename) {
	char *path = malloc(sizeof(char) * 200);

	strcpy(path, "/usr/share/rooibos/");
	strcat(path, filename);

	char *home = malloc(sizeof(char) * 200);
	char *home_tmp = utils_get(UTILS_FOLDER);
	strcpy(home, home_tmp);
	strcat(home, "/");
	strcat(home, filename);
	if (access(home, F_OK) == 0) {
		strcpy(path, home);
	}
	free(home);
	free(home_tmp);

	char *dev = malloc(sizeof(char) * 200);
	strcpy(dev, "./assets/");
	strcat(dev, filename);
	if (access(dev, F_OK) == 0) {
		strcpy(path, dev);
	}
	free(dev);

	return path;
}
