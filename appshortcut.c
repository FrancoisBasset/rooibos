#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ftw.h>
#include "appshortcut.h"
#include "utils.h"

static char *icon_to_search;
static char *result;

static int filter_icon(const char *path, const struct stat *sb, int typeflag);
static char* appshortcut_get_category(const char* categories);
static char* appshortcut_get_icon(char *icon);

appshortcut_t* appshortcut_get_app_shortcuts_by_category(const appshortcut_t* app_shortcuts, const char *category, int length, int *new_length) {
	appshortcut_t *new_app_shortcuts = malloc(sizeof(appshortcut_t) * length);
	*new_length = 0;

	for (int i = 0, j = 0; i < length; i++) {
		if (strcmp(app_shortcuts[i].category, category) == 0) {
			new_app_shortcuts[j] = app_shortcuts[i];
			j++;
			(*new_length)++;
		}
	}

	return new_app_shortcuts;
}

char** appshortcut_get_all_desktop_files(int *length) {
	int length1 = 0;
	int length2 = 0;
	int length3 = 0;
	char **desktop_files1 = appshortcut_get_desktop_files("/usr/share/applications/", &length1);
	char **desktop_files2 = appshortcut_get_desktop_files("/usr/local/share/applications/", &length2);
	char *local_apps_dir = utils_get_local_apps_dir();
	char **desktop_files3 = appshortcut_get_desktop_files(local_apps_dir, &length3);
	free(local_apps_dir);

	*length = length1 + length2 + length3;
	char **desktop_files = malloc(sizeof(char*) * (length1 + length2 + length3));

	int i;
	for (i = 0; i < length1; i++) {
		desktop_files[i] = desktop_files1[i];
	}
	free(desktop_files1);

	for (int j = 0; j < length2; i++, j++) {
		desktop_files[i] = desktop_files2[j];
	}
	free(desktop_files2);

	for (int j = 0; j < length3; i++, j++) {
		desktop_files[i] = desktop_files3[j];
	}
	free(desktop_files3);

	return desktop_files;
}

char** appshortcut_get_desktop_files(const char *foldername, int *length) {
	if (access(foldername, F_OK) != 0) {
		return malloc(0);
	}

	DIR *dir = opendir(foldername);

	*length = 0;

	const struct dirent *file;
	while ((file = readdir(dir)) != NULL) {
		if (strstr(file->d_name, ".desktop")) {
			(*length)++;
		}
	}

	rewinddir(dir);

	char **desktop_files = malloc(sizeof(char*) * *length);

	int i = 0;
	while ((file = readdir(dir)) != NULL) {
		if (strstr(file->d_name, ".desktop")) {
			desktop_files[i] = malloc(sizeof(char) * (strlen(foldername) + strlen(file->d_name) + 1));
			strcpy(desktop_files[i], foldername);
			strcat(desktop_files[i], file->d_name);
			i++;
		}
	}

	closedir(dir);

	return desktop_files;
}

appshortcut_t appshortcut_get_app_shortcut(const char *filename) {
	FILE *file = fopen(filename, "r");

	char *line = malloc(sizeof(char) * 100);
	char *key = malloc(sizeof(char) * 100);
	char *value = malloc(sizeof(char) * 100);
	char *name = malloc(sizeof(char) * 100);
	strcpy(name, "");
	char *exec = malloc(sizeof(char) * 100);
	strcpy(exec, "");
	char *categories = malloc(sizeof(char) * 100);
	char *icon = malloc(sizeof(char) * 100);
	strcpy(icon, "");

	strcpy(categories, "");

	while (fgets(line, 100, file)) {
		if (strstr(line, "=") == NULL) {
			continue;
		}

		sscanf(line, "%50[^=]=%50[^\n]", key, value);

		if (strcmp(key, "Name") == 0 && strcmp(name, "") == 0) {
			strcpy(name, value);
		} else if (strcmp(key, "Exec") == 0 && strcmp(exec, "") == 0) {
			for (int i = 0; i < strlen(value); i++) {
				if (value[i] == '%') {
					value[i] = '\0';
				}
			}
			
			strcpy(exec, value);
		} else if (strcmp(key, "Categories") == 0) {
			strcpy(categories, value);
		} else if (strcmp(key, "Icon") == 0 && strcmp(icon, "") == 0) {
			strcat(icon, "/");
			strcat(icon, value);
		} else if (strcmp(key, "Terminal") == 0 || strcmp(key, "NoDisplay") == 0) {
			if (strcmp(value, "true") == 0) {
				appshortcut_t null_shortcut = { NULL, NULL, NULL, NULL };
				free(line);
				free(key);
				free(value);
				free(name);
				free(exec);
				free(categories);
				free(icon);
				fclose(file);
				return null_shortcut;
			}
		}
	}

	icon = appshortcut_get_icon(icon);

	free(line);
	free(key);
	free(value);
	fclose(file);

	if (icon == NULL) {
		appshortcut_t null_shortcut = { NULL, NULL, NULL, NULL };
		free(name);
		free(exec);
		free(categories);
		free(icon);
		return null_shortcut;
	}

	char *category = appshortcut_get_category(categories);
	free(categories);

	appshortcut_t app_shortcut = { name, exec, category, icon };
	app_shortcut.file = malloc(sizeof(char) * 100);
	strcpy(app_shortcut.file, filename);

	return app_shortcut;
}

static int filter_icon(const char *path, const struct stat *sb, int typeflag) {
	if (strstr(path, ".png") != NULL && (strstr(path, "16x16") != NULL || strstr(path, "32x32") != NULL)) {
		return 0;
	}

	if (strstr(path, ".xpm") != NULL) {
		return 0;
	} 

	if (strstr(path, icon_to_search) != NULL) {
		result = malloc(sizeof(char) * (strlen(path) + 1));
		strcpy(result, path);
		return 1;
	}

	return 0;
}

static char* appshortcut_get_category(const char* categories) {
	char *new_category = NULL;

	char *categories_replacement[8][2] = {
		{ "Game;", "Games" },
		{ "System;", "System" },
		{ "Graphics;", "Graphics" },
		{ "Development;", "Development" },
		{ "Network;", "Network" },
		{ "AudioVideo;", "Multimedia" },
		{ "Office;", "Office" },
		{ "Settings;", "Settings" }
	};

	for (int i = 0; i < 8; i++) {
		if (strstr(categories, categories_replacement[i][0])) {
			new_category = malloc(sizeof(char) * (strlen(categories_replacement[i][1]) + 1));
			strcpy(new_category, categories_replacement[i][1]);
			return new_category;
		}
	}

	new_category = malloc(sizeof(char) * 6);
	strcpy(new_category, "Other");

	return new_category;
}

static char* appshortcut_get_icon(char *icon) {
	if (strlen(icon) == 0) {
		return NULL;
	}

	icon_to_search = icon;

	ftw("/usr/share/pixmaps", filter_icon, 50);
	if (result == NULL) {
		ftw("/usr/share/icons", filter_icon, 50);
		if (result == NULL) {
			char *local_icons_dir = utils_get_local_icons_dir();
			ftw(local_icons_dir, filter_icon, 50);
			free(local_icons_dir);
		}
	}

	if (result) {
		strcpy(icon, result);
		free(result);
		result = NULL;
	}

	return icon;
}
