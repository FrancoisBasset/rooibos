#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include "objects.h"
#include "window.h"
#ifdef WILLDEBUG
#include "debug.h"
#endif

static windows_t *ws;
static window_t *current;

window_t* window_init(Window id, const char *title, int x, int y, int width, int height) {
    window_t *new_window = malloc(sizeof(window_t));
    new_window->id = id;
    new_window->x = x;
    new_window->y = y;
    new_window->width = width;
    new_window->height = height;
    new_window->visible = 1;
    new_window->title = malloc(sizeof(char) * (strlen(title) + 1));
    strcpy(new_window->title, title);
    new_window->next = NULL;

    return new_window;
}

void windows_init(void) {
    ws = malloc(sizeof(windows_t));
    ws->first = NULL;
    ws->last = NULL;
    ws->length = 0;
}

void window_reset(void) {
    current = NULL;
}

window_t* window_next(void) {
    if (current == NULL) {
        if (ws->first == NULL) {
            return NULL;
        }

        current = ws->first;
        return current;
    }

    current = current->next;

    return current;
}

window_t* window_get(Window id) {
    window_reset();

    window_t *w = window_next();
    while (w != NULL) {
        if (w->id == id) {
            return w;
        }

		w = window_next();
    }

    return NULL;
}

#ifdef WILLDEBUG
void window_print(window_t* w) {
	const char visible = w->visible == 1 ? '+' : '-';
	
	debug("%c %d %s %d %d %d x %d", visible, w->id, w->title, w->x, w->y, w->width, w->height);
}

void windows_print() {
    window_reset();

    window_t *w = window_next();
    while (w != NULL) {
        window_print(w);

		w = window_next();
    }
}
#endif

void window_add(window_t *w) {
    if (window_get(w->id) != NULL) {
        return;
    }

    if (ws->first == NULL) {
        ws->first = w;
        ws->last = w;
    } else {
        ws->last->next = w;
        ws->last = w;
    }

    ws->length++;
}

void window_update(Window id, const char *property, const void *value) {
    window_t *w = NULL;

    window_reset();
    while (w = window_next()) {
        if (w->id == id) {
            if (strcmp(property, "title") == 0) {
                free(w->title);
                w->title = malloc(sizeof(char) * (strlen(value) + 1));
                strcpy(w->title, value);

                int width = 200;
                int length = (int) strlen(w->title) + 1;
                while (width >= 200) {
                    length--;
                    width = XTextWidth(font_struct, w->title, length);
                }
                
                w->title[length] = '\0';
            } else if (strcmp(property, "x") == 0) {
                w->x = *((int *) value);
            } else if (strcmp(property, "y") == 0) {
                w->y = *((int *) value);
            } else if (strcmp(property, "width") == 0) {
                w->width = *((int *) value);
            } else if (strcmp(property, "height") == 0) {
                w->height = *((int *) value);
            }
            break;
        }
    }
}

void window_delete(Window id) {
	window_reset();
	
	window_t *w = window_next();

    if (w == NULL) {
        return;
    }

    if (w->id == id) {
        ws->first = w->next;
        window_free(w);
        ws->length--;
        return;
    }
    
    while (w != NULL) {
        if (w->next->id == id) {
            window_t* next = w->next;
            w->next = w->next->next;
            window_free(next);
            ws->length--;
            break;
        }

		w = window_next();
    }
}

void window_free(window_t* w) {
    free(w->title);
    free(w);
}

void windows_free(void) {
    window_reset();
    
    window_t *w = window_next();
    while (w != NULL) {
        window_free(w);

		w = window_next();
    }

    free(ws);
}

int window_length(void) {
    return ws->length;
}

void window_show_all_visible(void) {
    window_reset();

    window_t *w = window_next();
    while (w != NULL) {
        if (w->visible == 1) {
            XMapWindow(display, w->id);
        }
		
		w = window_next();
    }
}

void window_hide_all_visible(void) {
    window_reset();

    window_t *w = window_next();
    while (w != NULL) {
        if (w->visible == 1) {
            XUnmapWindow(display, w->id);
        }

		w = window_next();
    }
}
