#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "objects.h"
#include "prompt.h"
#include "rooibos.h"
#include "cache.h"

int prompt_active;
char *command;
int cursor_index = 0;

char **history = NULL;
int history_index = -1;
int history_length = 0;

void prompt_init(void) {
	command = malloc(sizeof(char) * 1000);
	strcpy(command, "");

	history = cache_get_history(&history_length);
}

void prompt_show(void) {
	XFillRectangle(display, window, gc_text_black, 0, screen_height - 50, screen_width, 50);
	XDrawString(display, window, gc_text_white, 0, screen_height - 23, "$ ", 2);
	XDrawString(display, window, gc_text_white, 10, screen_height - 23, command, (int) strlen(command));

	int width = XTextWidth(font_struct, command, cursor_index);
	XDrawLine(display, window, gc_text_white, 10 + width, screen_height - 32, 10 + width, screen_height - 22);

	prompt_active = 1;
}

void prompt_hide(void) {
	prompt_active = 0;
	history_index = -1;

	strcpy(command, "");
	cursor_index = 0;
	XEvent event = { .type = Expose };
	XSendEvent(display, window, 0, ExposureMask, &event);
}

void prompt_exec(void) {
	if (fork() == 0) {
		if (strcmp(command, "") == 0) {
			execlp("xterm", "xterm", "-T", "xterm", NULL);
		} else {
			char *xterm_option = malloc(sizeof(char) * (strlen(command) + 16));
			strcpy(xterm_option, command);
			if (strcmp(command, "ls") == 0) {
				strcat(xterm_option, " --color");
			}
			strcat(xterm_option, ";bash");
			execlp("xterm", "xterm", "-T", "xterm", "-e", xterm_option, NULL);
			free(xterm_option);
		}
	}
	if (strcmp(command, "") != 0) {
		cache_add_history(command);
		history = cache_get_history(&history_length);
		history_index = -1;
	}
	
	prompt_hide();
}

void prompt_write(const char *typed) {
	if (strlen(typed) > 0) {
		char *tmp = malloc(sizeof(char) * (strlen(command) + 2));
		strcpy(tmp, command);
		tmp[cursor_index] = '\0';
		
		strcat(tmp, typed);
		strcat(tmp, command + cursor_index);
		strcpy(command, tmp);

		free(tmp);

		prompt_move_right();
		XEvent event = { .type = Expose };
		XSendEvent(display, window, 0, ExposureMask, &event);
	}
}

void prompt_erase(void) {
	if (strlen(command) > 0 && cursor_index > 0) {
		char *tmp = malloc(sizeof(char) * (strlen(command) + 1));
		strcpy(tmp, command);
		tmp[cursor_index - 1] = '\0';
		
		strcat(tmp, command + cursor_index);
		strcpy(command, tmp);

		free(tmp);

		prompt_move_left();
		XEvent event = { .type = Expose };
		XSendEvent(display, window, 0, ExposureMask, &event);
	}
}

void prompt_delete(void) {
	if (strlen(command) > 0 && cursor_index <= strlen(command) - 1) {
		char *tmp = malloc(sizeof(char) * (strlen(command) + 1));
		strcpy(tmp, command);
		tmp[cursor_index] = '\0';
		
		strcat(tmp, command + cursor_index + 1);
		strcpy(command, tmp);

		free(tmp);

		XEvent event = { .type = Expose };
		XSendEvent(display, window, 0, ExposureMask, &event);
	}
}

void prompt_move_left(void) {
	if (cursor_index != 0) {
		cursor_index--;
		XEvent event = { .type = Expose };
		XSendEvent(display, window, 0, ExposureMask, &event);
	}
}

void prompt_move_right(void) {
	if (cursor_index != strlen(command)) {
		cursor_index++;
		XEvent event = { .type = Expose };
		XSendEvent(display, window, 0, ExposureMask, &event);
	}
}

void prompt_up(void) {
	if (history_length == 0) {
		return;
	}

	if (history_index == 0) {
		return;
	}

	if (history_index == -1) {
		history_index = history_length;
	}

	history_index--;
	strcpy(command, history[history_index]);
	cursor_index = (int) strlen(command);

	XEvent event = { .type = Expose };
	XSendEvent(display, window, 0, ExposureMask, &event);
}

void prompt_down(void) {
	if (history_index == -1) {
		return;
	}

	if (history_index < history_length - 1) {
		history_index++;
		strcpy(command, history[history_index]);
		cursor_index = (int) strlen(command);
	} else {
		strcpy(command, "");
		cursor_index = (int) strlen(command);
		history_index = -1;
	}

	XEvent event = { .type = Expose };
	XSendEvent(display, window, 0, ExposureMask, &event);
}

void prompt_on_key_press(XKeyEvent key_event, KeySym key_sym) {
	switch (key_sym) {
		case XK_Escape:
		case XK_Control_L:
			prompt_hide();
			return;
		case XK_Return:
			prompt_exec();
			return;
		case XK_BackSpace:
			prompt_erase();
			return;
		case XK_Left:
			prompt_move_left();
			return;
		case XK_Right:
			prompt_move_right();
			return;
		case XK_Up:
			prompt_up();
			return;
		case XK_Down:
			prompt_down();
			return;
		case XK_Tab:
			prompt_write(" ");
			return;
		case XK_Delete:
			prompt_delete();
			return;
		default:
			break;
	}

	XIM xim = XOpenIM(display, NULL, NULL, NULL);
	XIC xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, window, NULL);
	char *typed = malloc(sizeof(char) * 10);
	strcpy(typed, "");
	
	XmbLookupString(xic, &key_event, typed, sizeof(typed), NULL, NULL);

	prompt_write(typed);
	free(typed);
}
