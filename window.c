#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objects.h"
#include "window.h"

static struct windows *ws;
static struct window *current;

struct window* window_init(int id, const char *title, int x, int y, int width, int height) {
    struct window *new_window = malloc(sizeof(struct window));
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
    ws = malloc(sizeof(struct windows));
    ws->first = NULL;
    ws->last = NULL;
    ws->length = 0;
}

void window_reset(void) {
    current = NULL;
}

struct window* window_next(void) {
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

struct window* window_get(int id) {
    struct window *w = NULL;

    window_reset();
    while (w = window_next()) {
        if (w->id == id) {
            return w;
        }
    }

    return NULL;
}

void window_print(FILE *debug, struct window* w) {
    char visible = '-';
    if (w->visible == 1) {
        visible = '+';
    }
    fprintf(debug, "%c <%d> <%s> <%d,%d> <%dx%d>\n", visible, w->id, w->title, w->x, w->y, w->width, w->height);
}

void windows_print(FILE *debug) {
    struct window *w = NULL;
    
    window_reset();
    while (w = window_next()) {
        window_print(debug, w);
    }
}

void window_add(struct window *w) {
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

void window_update(int id, const char *property, const void *value) {
    struct window *w = NULL;

    window_reset();
    while (w = window_next()) {
        if (w->id == id) {
            if (strcmp(property, "title") == 0) {
                free(w->title);
                w->title = malloc(sizeof(char) * (strlen(value) + 1));
                strcpy(w->title, value);

                Font font_text_button = XLoadFont(display, "-*-times-*-r-*-*-14-*-*-*-*-*-*-*");
                XFontStruct *fontstruct = XQueryFont(display, font_text_button);

                int width = 200;
                int length = (int) strlen(w->title) + 1;
                while (width >= 200) {
                    length--;
                    width = XTextWidth(fontstruct, w->title, length);
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

void window_delete(int id) {
    if (ws->first == NULL) {
        return;
    }

    struct window *w = ws->first;
    if (w->id == id) {
        ws->first = w->next;
        window_free(w);
        ws->length--;
        return;
    }
    
    window_reset();
    while (w = window_next()) {
        if (w->next->id == id) {
            struct window* next = w->next;
            w->next = w->next->next;
            window_free(next);
            ws->length--;
            break;
        }
    }
}

void window_free(struct window* w) {
    free(w->title);
    free(w);
}

void windows_free(void) {
    window_reset();
    if (ws->first == NULL) {
        free(ws);
        return;
    }

    window_reset();
    struct window *w = ws->first;
    struct window *next = NULL;

    do {
        next = w->next;
        window_free(w);
    } while ((w = next) != NULL);

    free(ws);
}

struct windows* windows_get(void) {
    return ws;
}
