#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>

struct passwd *pw = NULL;

char* utils_get_login(void) {
	if (pw == NULL) {
		pw = getpwnam(getlogin());
	}

	return pw->pw_name;
}

char* utils_get_user_home(void) {
	if (pw == NULL) {
		pw = getpwnam(getlogin());
	}

	return pw->pw_dir;
}

char* utils_get_local_apps_dir(void) {
	const char *user_home = utils_get_user_home();
	
	char *local_apps_dir = malloc(sizeof(char) * (28 + strlen(user_home)));
	strcpy(local_apps_dir, user_home);
	strcat(local_apps_dir, "/.local/share/applications/");

	return local_apps_dir;
}

char* utils_get_local_icons_dir(void) {
	const char *user_home = utils_get_user_home();

	char *local_icons_dir = malloc(sizeof(char) * (21 + strlen(user_home)));
	strcpy(local_icons_dir, user_home);
	strcat(local_icons_dir, "/.local/share/icons/");

	return local_icons_dir;
}

char* utils_get_cache_path(void) {
	const char *user_home = utils_get_user_home();
	
	char *cache_path = malloc(sizeof(char) * (strlen(user_home) + 19));
	strcpy(cache_path, user_home);
	strcat(cache_path, "/.rooibos/cache.db");

	return cache_path;
}

char* utils_get_logo_path(void) {
	const char *user_home = utils_get_user_home();
	
	char *logo_path = malloc(sizeof(char) * (strlen(user_home) + 19));
	strcpy(logo_path, user_home);
	strcat(logo_path, "/.rooibos/logo.svg");

	return logo_path;
}

char* utils_get_wallpaper_path(void) {
	const char *user_home = utils_get_user_home();
	
	char *wallpaper_path = malloc(sizeof(char) * (strlen(user_home) + 24));
	strcpy(wallpaper_path, user_home);
	strcat(wallpaper_path, "/.rooibos/wallpaper.png");

	return wallpaper_path;
}

int utils_is_root(void) {
	if (getuid() == 0) {
		return 1;
	} else {
		return 0;
	}
}
