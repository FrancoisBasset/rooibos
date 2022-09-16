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

struct appshortcut* appshortcut_get_app_shortcuts(int *_length) {
	int length1 = 0;
	int length2 = 0;
	int length3 = 0;

	char **desktop_files1 = appshortcut_get_desktop_files("/usr/share/applications/", &length1);
	char **desktop_files2 = appshortcut_get_desktop_files("/usr/local/share/applications/", &length2);

	char *user_home = utils_get_user_home();
	char **desktop_files3 = appshortcut_get_desktop_files(user_home, &length3);

	int length = length1 + length2 + length3;
	char **desktop_files = malloc(sizeof(char*) * length);

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

	struct appshortcut* app_shortcuts = malloc(sizeof(struct appshortcut) * length);

	int real_length = 0;
	for (i = 0; i < length; i++) {
		struct appshortcut app_shortcut = appshortcut_get_app_shortcut(desktop_files[i]);

		if (app_shortcut.name != NULL) {
			app_shortcuts[real_length] = app_shortcut;
			real_length++;
		}

		free(desktop_files[i]);
	}

	free(desktop_files);

	*_length = real_length;

	return app_shortcuts;
}

static char** appshortcut_get_desktop_files(char *foldername, int *length) {
	if (access(foldername, F_OK) != 0) {
		return malloc(sizeof(char*) * 0);
	}

	DIR *dir = opendir(foldername);

	*length = 0;

	struct dirent *file;
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

static struct appshortcut appshortcut_get_app_shortcut(char *filename) {
	FILE *file = fopen(filename, "r");

	char *line = malloc(sizeof(char) * 100);
	char *key = malloc(sizeof(char) * 100);
	char *value = malloc(sizeof(char) * 100);
	char *name = malloc(sizeof(char) * 100);
	char *exec = malloc(sizeof(char) * 100);
	char *categories = malloc(sizeof(char) * 100);
	char *icon = malloc(sizeof(char) * 100);

	strcpy(categories, "");

	while (fgets(line, 100, file)) {
		if (strstr(line, "=") == NULL) {
			continue;
		}

		sscanf(line, "%50[^=]=%50[^\n]", key, value);

		if (strcmp(key, "Name") == 0) {
			strcpy(name, value);
		} else if (strcmp(key, "Exec") == 0) {
			strcpy(exec, value);
		} else if (strcmp(key, "Categories") == 0) {
			strcpy(categories, value);
		} else if (strcmp(key, "Icon") == 0) {
			strcpy(icon, value);
		} else if (strcmp(key, "Terminal") == 0) {
			if (strcmp(value, "true") == 0) {
				struct appshortcut null_shortcut = { NULL, NULL, NULL, NULL };
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
		struct appshortcut null_shortcut = { NULL, NULL, NULL, NULL };
		free(name);
		free(exec);
		free(categories);
		free(icon);
		return null_shortcut;
	}

	char *categorie = appshortcut_get_categorie(categories);
	free(categories);

	struct appshortcut app_shortcut = { name, exec, categorie, icon };
	app_shortcut.file = malloc(sizeof(char) * 100);
	strcpy(app_shortcut.file, filename);

	return app_shortcut;
}

static int filter_icon(const char *path, const struct stat *sb, int typeflag) {
	if (strstr(path, icon_to_search)) {
		result = malloc(sizeof(char) * (strlen(path) + 1));
		strcpy(result, path);
		return 1;
	}

	return 0;
}

static char* appshortcut_get_categorie(char* categories) {
	char *new_categorie = NULL;

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
			new_categorie = malloc(sizeof(char) * (strlen(categories_replacement[i][1]) + 1));
			strcpy(new_categorie, categories_replacement[i][1]);
			return new_categorie;
		}
	}

	new_categorie = malloc(sizeof(char) * 6);
	strcpy(new_categorie, "Other");

	return new_categorie;
}

static char* appshortcut_get_icon(char *icon) {
	if (strlen(icon) == 0) {
		return NULL;
	}

	if (icon[0] == '/') {
		return icon;
	}

	icon_to_search = icon;

	ftw("/usr/share/pixmaps", filter_icon, 50);
	if (result == NULL) {
		ftw("/usr/share/icons", filter_icon, 50);
	}

	if (result) {
		strcpy(icon, result);
		free(result);
		result = NULL;
	}

	return icon;
}
