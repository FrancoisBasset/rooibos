#ifndef SHORTCUT_H
#define SHORTCUT_H
typedef struct shortcut_t {
	char* title;
	int type;
	int x;
	int y;
	int width;
	int height;
} shortcut_t;
shortcut_t* shortcut_get_files(void);
shortcut_t* shortcut_get_shortcuts(void);
void shortcut_show_all(void);
void shortcut_show_folder(shortcut_t shortcut);
void shortcut_show_file(shortcut_t shortcut);
int shortcut_on_hover(int x, int y);
void shortcut_on_press(int x, int y);
#endif