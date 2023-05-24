#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "shortcut.h"
#include "objects.h"
#include "utils.h"
#include "event.h"

shortcut_t *shortcuts = NULL;
int length = 0;
char *current_dir = NULL;

shortcut_t* shortcut_get_files(void) {
	DIR *dir = opendir(current_dir);

	int capacity = 2;
	length = 0;
	int i = 0;

	shortcut_t *files = malloc(sizeof(shortcut_t) * capacity);

	struct stat details;
	const struct dirent *file;
	while ((file = readdir(dir)) != NULL) {
		if (file->d_name[0] != '.') {
			if (capacity == length) {
				capacity += 2;
				files = realloc(files, sizeof(shortcut_t) * capacity);
			}

			char type = ' ';
			char *path = malloc(sizeof(char) * (strlen(current_dir) + strlen(file->d_name) + 2));
			strcpy(path, current_dir);
			strcat(path, "/");
			strcat(path, file->d_name);

			stat(path, &details);
			if (S_ISREG(details.st_mode)) {
				type = 'f';
			} else if (S_ISDIR(details.st_mode)) {
				type = 'd';
			}

			free(path);

			files[i] = (shortcut_t) {
				.title = strdup(file->d_name),
				.type = type
			};

			length++;
			i++;
		}
	}

	closedir(dir);

	return files;
}

shortcut_t* shortcut_get_shortcuts(void) {
	if (current_dir == NULL) {
		current_dir = strdup(utils_get(UTILS_USER_HOME));
	}

	shortcuts = shortcut_get_files();

	int width = 100;
	int height = 100;
	int x = 20;
	int y = 20;

	for (int i = 0; i < length; i++) {
		if (shortcuts[i].type == 'd') {
			width = 100;
		} else {
			width = 80;
		}

		shortcuts[i].x = x;
		shortcuts[i].y = y;
		shortcuts[i].width = width;
		shortcuts[i].height = height;

		x += 120;
		if (x > screen_width) {
			x = 20;
			y += width + 60;
		}
	}

	return shortcuts;
}

void shortcut_show_all(void) {
	if (shortcuts == NULL) {
		shortcuts = shortcut_get_shortcuts();
	}
	
	for (int i = 0; i < length; i++) {
		if (shortcuts[i].type == 'd') {
			shortcut_show_folder(shortcuts[i]);
		} else if (shortcuts[i].type == 'f') {
			shortcut_show_file(shortcuts[i]);
		}

		int width = 101;
		int title_length = (int) strlen(shortcuts[i].title) + 1;

		while (width > 100) {
			width = XTextWidth(font_struct, shortcuts[i].title, title_length);
			title_length--;
		}

		XFillRectangle(display, back, gc_text_black, shortcuts[i].x - 5, shortcuts[i].y + shortcuts[i].height + 5, width + 5, 20);
		XDrawString(display, back, gc_text_white, shortcuts[i].x, shortcuts[i].y + shortcuts[i].height + 20, shortcuts[i].title, title_length);
	}
}

void shortcut_show_folder(shortcut_t shortcut) {
	short x = (short) shortcut.x;
	short y = (short) shortcut.y;

	XPoint points[6] = {
		{ .x = x, .y = y},
		{ .x = x + 40, .y = y},
		{ .x = x + 40, .y = y + 10},
		{ .x = x + (short) shortcut.width, .y = y + 10},
		{ .x = x + (short) shortcut.width, .y = y + (short) shortcut.height},
		{ .x = x, .y = y + (short) shortcut.height}
	};

	XFillPolygon(display, back, gc_folder, points, 6, Nonconvex, CoordModeOrigin);
}

void shortcut_show_file(shortcut_t shortcut) {
	short x = (short) shortcut.x;
	short y = (short) shortcut.y;

	XPoint points[5] = {
		{ .x = x, .y = y},
		{ .x = x + 60, .y = y},
		{ .x = x + (short) shortcut.width, .y = y + 20},
		{ .x = x + (short) shortcut.width, .y = y + (short) shortcut.height},
		{ .x = x, .y = y + (short) shortcut.height},
	};

	XFillPolygon(display, back, gc_right_panel_hover, points, 5, Nonconvex, CoordModeOrigin);
}

int shortcut_on_hover(int x, int y) {
	for (int i = 0; i < length; i++) {
		if (x >= shortcuts[i].x && x <= shortcuts[i].x + shortcuts[i].width && y >= shortcuts[i].y && y <= shortcuts[i].y + shortcuts[i].height) {
			XDefineCursor(display, window, hand_cursor);
			return i;
		}
	}

	return -1;
}

void shortcut_on_press(int x, int y) {
	int i = shortcut_on_hover(x, y);

	if (i == -1) {
		return;
	}

	char *name = malloc(sizeof(char) * (strlen(current_dir) + strlen(shortcuts[i].title) + 2));
	strcpy(name, current_dir);
	strcat(name, "/");
	strcat(name, shortcuts[i].title);

	if (shortcuts[i].type == 'd') {
		if (fork() == 0) {
			execlp("xfe", "xfe", name, NULL);
		}
	} else if (shortcuts[i].type == 'f') {
		if (fork() == 0) {
			execlp("xdg-open", "xdg-open", name, NULL);
		}
	}
}
