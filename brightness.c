#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objects.h"
#include "brightness.h"

int brightness_get(void) {
	FILE *file = fopen("/sys/class/backlight/intel_backlight/brightness", "r");

	int brightness;
	int success = fscanf(file, "%d", &brightness);

	fclose(file);

	return brightness / 1200;
}

void brightness_set(int percentage) {
	percentage *= 1200;

	FILE *file = fopen("/sys/class/backlight/intel_backlight/brightness", "w");

	fprintf(file, "%d", percentage);

	fclose(file);
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
	char *text = malloc(sizeof(char) * 100);
	sprintf(text, "Brightness : %d%%", brightness);

	XDrawString(display, window, gc_text_white, 10, 30, text, strlen(text));
}
