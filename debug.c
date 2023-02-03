#include <stdio.h>
#include "debug.h"

FILE *debug;

void debug_init(void) {
    debug = fopen("./debug.txt", "w");
}

void debug_free(void) {
    fclose(debug);
}
