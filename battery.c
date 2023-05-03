#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "battery.h"

int battery_get_level(void) {
	FILE *file = fopen("/sys/class/power_supply/BAT1/capacity", "r");

	int battery = 0;

	if (file != NULL) {
		fscanf(file, "%d", &battery);
		fclose(file);
	}

	return battery;
}

int battery_get_status(void) {
	FILE *file = fopen("/sys/class/power_supply/BAT1/status", "r");

	int code = 0;

	if (file != NULL) {
		char *status = malloc(sizeof(char) * 12);
		fscanf(file, "%12s", status);
		fclose(file);

		if (strcmp(status, "Full") == 0) {
			code = 2;
		} else if (strcmp(status, "Charging") == 0) {
			code = 1;
		}

		free(status);
	}

	return code;
}
