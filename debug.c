#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "debug.h"

void debug(char *format, ...) {
	FILE *debug_file = fopen("./debug", "a");
	int intVal;
	double doubleVal;
	long longVal;
	char charVal;
	char *stringVal;

	va_list adpar;
	va_start(adpar, format);

	char *new_format = malloc(sizeof(char) * (strlen(format) + 1));
	strcpy(new_format, format);

	char *symbol = strtok(new_format, " ");
	do {
		if (strcmp(symbol, "%d") == 0) {
			intVal = va_arg(adpar, int);
			fprintf(debug_file, "%d", intVal);
		} else if (strcmp(symbol, "%ld") == 0) {
			longVal = va_arg(adpar, long);
			fprintf(debug_file, "%ld", longVal);
		} else if (strcmp(symbol, "%f") == 0) {
			doubleVal = va_arg(adpar, double);
			fprintf(debug_file, "%f", doubleVal);
		} else if (strcmp(symbol, "%c") == 0) {
			charVal = (char) va_arg(adpar, int);
			fprintf(debug_file, "%c", charVal);
		} else if (strcmp(symbol, "%s") == 0) {
			stringVal = va_arg(adpar, char*);
			fprintf(debug_file, "%s", stringVal);
		} else {
			fprintf(debug_file, "%s", symbol);
		}

		fprintf(debug_file, " ");
    } while ((symbol = strtok(NULL, " ")) != NULL);

	va_end(adpar);

	fprintf(debug_file, "\n");

	fclose(debug_file);
}
