#ifndef WINDOW_H
#define WINDOW_H
struct window {
    int id;
    char *title;
    int x;
    int y;
    int width;
    int height;
    int visible;
    struct window *next;
};

struct windows {
    struct window *first;
    struct window *last;
    int length;
};

struct window* window_init(int id, const char *title, int x, int y, int width, int height);
void windows_init(void);

void window_reset(void);
struct window* window_next(void);
struct window* window_get(int id);
void window_print(FILE *debug, struct window* w);
void windows_print(FILE *debug);
struct windows* windows_get(void);

void window_add(struct window* w);
void window_update(int id, const char *property, const void *value);
void window_delete(int id);

void window_free(struct window* w);
void windows_free(void);
#endif
