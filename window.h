#ifndef WINDOW_H
#define WINDOW_H
#include <stdio.h>
struct window_t {
    int id;
    char *title;
    int x;
    int y;
    int width;
    int height;
    int visible;
    struct window_t *next;
};

struct windows_t {
    struct window_t *first;
    struct window_t *last;
    int length;
};

struct window_t* window_init(int id, const char *title, int x, int y, int width, int height);
void windows_init(void);

void window_reset(void);
struct window_t* window_next(void);
struct window_t* window_get(int id);
void window_print(FILE *debug, struct window_t* w);
void windows_print(FILE *debug);
struct windows_t* windows_get(void);

void window_add(struct window_t* w);
void window_update(int id, const char *property, const void *value);
void window_delete(int id);

void window_free(struct window_t* w);
void windows_free(void);
#endif
