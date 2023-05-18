#ifndef WINDOW_H
#define WINDOW_H
#include <stdio.h>
#include "decorator.h"
typedef struct window_t {
	Window id;
	char *title;
	int x;
	int y;
	int width;
	int height;
	int visible;
	decorator_t decorator;
	struct window_t *next;
} window_t;

typedef struct windows_t {
	window_t *first;
	window_t *last;
	int length;
} windows_t;

window_t* window_init(Window id, const char *title, int x, int y, int width, int height);
void windows_init(void);

void window_reset(void);
window_t* window_next(void);
window_t* window_get(Window id);

#ifdef WILLDEBUG
void window_print(window_t* w);
void windows_print();
#endif

int window_length(void);

void window_add(window_t* w);
void window_update(Window id, const char *property, const void *value);
void window_delete(Window id);

void window_free(window_t* w);
void windows_free(void);

void window_show_all_visible(void);
void window_hide_all_visible(void);
#endif
