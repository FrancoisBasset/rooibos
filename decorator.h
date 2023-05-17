#ifndef DECORATOR_H
#define DECORATOR_H
typedef struct decorator_t {
	int x;
	int y;
	int width;
	int min_start;
	int min_end;
	int max_start;
	int max_end;
	int close_start;
	int close_end;
} decorator_t;

void decorator_show_all(void);
int decorator_on_hover(int x, int y);
void decorator_on_press(int x, int y);
Window decorator_get_selected(void);
void decorator_set_selected_null();
#endif