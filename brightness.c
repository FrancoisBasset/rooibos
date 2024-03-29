#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objects.h"
#include "utils.h"
#include "icon.h"
#include "brightness.h"

int brightness_get(void) {
	FILE *file = fopen("/sys/class/backlight/intel_backlight/brightness", "r");

	int brightness = 0;

	if (file != NULL) {
		fscanf(file, "%d", &brightness);
		fclose(file);
	}

	return brightness / 1200;
}

void brightness_set(int percentage) {
	percentage *= 1200;

	FILE *file = fopen("/sys/class/backlight/intel_backlight/brightness", "w");

	if (file != NULL) {
		fprintf(file, "%d", percentage);
		fclose(file);
	}
}

void brightness_up(void) {
	int brightness = brightness_get();

	if (brightness != 100) {
		brightness_set(brightness + 5);
	}
}

void brightness_down(void) {
	int brightness = brightness_get();
	
	if (brightness != 0) {
		brightness_set(brightness - 5);
	}
}

void brightness_show(void) {
	int brightness = brightness_get();
	char *text = malloc(sizeof(char) * 5);
	sprintf(text, "%d%%", brightness);

	char *brightness_logo = utils_get(UTILS_BRIGHTNESS);

	Pixmap panel = XCreatePixmap(display, window, 200, 60, screen_depth);
	Pixmap brightness_pixmap = icon_get_pixmap(brightness_logo, 40, 40);

	free(brightness_logo);

	XFillRectangle(display, panel, gc_icon, 0, 0, 200, 60);
	XCopyArea(display, brightness_pixmap, panel, XDefaultGCOfScreen(screen), 0, 0, 40, 40, 5, 10);

	const int width = (int) (12 * (brightness * 0.1));
	XFillRectangle(display, panel, gc_fore_green, 50, 25, width, 10);

	XDrawString(display, panel, gc_text_white, 170, 35, text, (int) strlen(text));

	free(text);

	const int x = (screen_width - 200) / 2;
	const int y = (screen_height / 2);
	XCopyArea(display, panel, window, XDefaultGCOfScreen(screen), 0, 0, 200, 60, x, y);
}
