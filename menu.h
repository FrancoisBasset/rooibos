#ifndef MENU_H
#define MENU_H
typedef struct menu_t {
    int x;
    int y;
    int width;
    int height;
    int *category_index;
    int is_showed;
} menu_t;

extern menu_t menu;

void menu_init(void);
void menu_show(void);
void menu_clear(void);
#endif