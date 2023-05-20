#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <dirent.h>
#include "utils.h"

struct passwd *pw = NULL;

static char* utils_get_wallpaper_to_use();
static char* utils_get_logo_to_use();
static char* utils_find_first(const char *foldername, const char *filename);
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
		case UTILS_WALLPAPER_TO_USE:
			result = utils_get_wallpaper_to_use();
			break;
		case UTILS_NEW_TERMINAL:
			result = utils_get_assets("new_terminal.png");
			break;
		case UTILS_LOGOUT:
			result = utils_get_assets("logout.png");
			break;
		case UTILS_LOCK:
			result = utils_get_assets("lock.png");
			break;
		case UTILS_COMPUTER:
			result = utils_get_assets("computer.png");
			break;
		case UTILS_SOUND:
			result = utils_get_assets("sound.png");
			break;
		case UTILS_BRIGHTNESS:
			result = utils_get_assets("brightness.png");
			break;
		case UTILS_CHARGING:
			result = utils_get_assets("charging.png");
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

	char *folder = utils_get(UTILS_FOLDER);
	char *local = utils_find_first(folder, "wallpaper.");
	free(folder);
	if (local != NULL) {
		strcpy(wallpaper_file_path, local);
		free(local);
	}


	char *dev = utils_find_first("./assets/", "wallpaper.");
	if (dev != NULL) {
		strcpy(wallpaper_file_path, dev);
		free(dev);
	}

	return wallpaper_file_path;
}

static char* utils_get_logo_to_use() {
	char *logo_file_path = malloc(sizeof(char) * 200);
	strcpy(logo_file_path, "/usr/share/rooibos/logo.svg");

	char *folder = utils_get(UTILS_FOLDER);
	char *local = utils_find_first(folder, "logo.");
	free(folder);
	if (local != NULL) {
		strcpy(logo_file_path, local);
		free(local);
	}

	char *dev = utils_find_first("./assets/", "logo.");
	if (dev != NULL) {
		strcpy(logo_file_path, dev);
		free(dev);
	}

	return logo_file_path;
}

static char* utils_get_assets(const char *filename) {
	char *path = malloc(sizeof(char) * 200);

	strcpy(path, "/usr/share/rooibos/");
	strcat(path, filename);

	char *dev = utils_find_first("./assets/", filename);
	if (dev != NULL) {
		strcpy(path, dev);
		free(dev);
	}

	return path;
}

static char* utils_find_first(const char *foldername, const char *filename) {
	if (access(foldername, F_OK) != 0) {
		return NULL;
	}

	DIR *dir = opendir(foldername);

	char *result = NULL;

	const struct dirent *file;
	while ((file = readdir(dir)) != NULL) {
		if (strstr(file->d_name, filename) != NULL) {
			result = malloc(sizeof(char) * ((int) strlen(file->d_name) + strlen(foldername) + 1));
			strcpy(result, foldername);
			strcat(result, file->d_name);
			break;
		}
	}

	closedir(dir);

	return result;
}
