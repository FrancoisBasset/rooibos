#ifndef ICON_H
#define ICON_H
typedef struct icon_t {
	char *filename;
	char* name;
	char* exec;
	int x;
	int y;
	int x_press;
	int y_press;
	int width;
	int height;
	Pixmap pixmap;
} icon_t;

void icons_init(void);

void icons_draw(void);
void icon_draw(icon_t icon);

Pixmap icon_get_pixmap(const char *filename, int width, int height);

int icons_on_hover(int x, int y);
void icons_on_press(int x, int y);
void icon_scroll_up(void);
void icon_scroll_down(void);
#endif
