#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <security/pam_appl.h>
#include <X11/keysym.h>
#include "lock.h"
#include "objects.h"
#include "event.h"
#include "utils.h"
#include "icon.h"

int button_x;
int button_y;

int conversation(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
	*resp = calloc(num_msg, sizeof(struct pam_response));
	if (*resp == NULL) {
		return PAM_BUF_ERR;
	}

	for (int i = 0; i < num_msg; i++) {
		if (msg[i]->msg_style == PAM_PROMPT_ECHO_OFF) {
			(*resp)[i].resp = strdup((const char*) appdata_ptr);
			(*resp)[i].resp_retcode = 0;
		}
	}

	return PAM_SUCCESS;
}

int authenticate(const char *username, const char *password) {
	pam_handle_t *pamh = NULL;
	int retval;

	struct pam_conv pam_conversation = {
		.conv = conversation,
		.appdata_ptr = (void*) password
	};

	retval = pam_start("login", username, &pam_conversation, &pamh);
	if (retval != PAM_SUCCESS) {
		return 0;
	}

	retval = pam_authenticate(pamh, 0);
	if (retval != PAM_SUCCESS) {
		pam_end(pamh, retval);
		return 0;
	}

	pam_end(pamh, PAM_SUCCESS);
	return 1;
}

int button_is_hover(int x, int y) {
	return x >= button_x && x <= button_x + 100 && y >= button_y && y <= button_y + 30;
}

void lock_start(void) {
	XFillRectangle(display, window, gc_decorator, 0, 0, screen_width, screen_height);

	const int logo_x = (screen_width - 100) / 2;
	const int logo_y = (screen_height - 100) / 2;
	char *logo_computer = utils_get(UTILS_COMPUTER);
	Pixmap pixmap = icon_get_pixmap(logo_computer, 100, 100);
	free(logo_computer);
	XCopyArea(display, pixmap, window, XDefaultGCOfScreen(screen), 0, 0, 100, 100, logo_x, logo_y);

	const int x = (screen_width - 300) / 2;
	const int y = (screen_height / 3) * 2;

	char *username = utils_get(UTILS_LOGIN);
	const int username_width = XTextWidth(font_struct, username, (int) strlen(username));
	const int username_x = screen_width / 2 - username_width / 2;
	XDrawString(display, window, gc_text_white, username_x, y - 50, username, (int) strlen(username));

	XDrawRectangle(display, window, gc_text_white, x, y, 300, 40);

	button_x = (screen_width - 100) / 2;
	button_y = y + 80;
	XFillRectangle(display, window, gc_text_white, button_x, button_y, 100, 30);
	const int button_label_width = XTextWidth(font_struct, "Login", 5);
	const int button_label_x = (screen_width - button_label_width) / 2;
	XDrawString(display, window, gc_text_black, button_label_x, y + 100, "Login", 5);

	char *password = malloc(sizeof(char) * 100);
	sprintf(password, "");

	char* masked_password = malloc(sizeof(char) * 100);
	sprintf(masked_password, "");

	XIM xim = XOpenIM(display, NULL, NULL, NULL);
	XIC xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, window, NULL);
	char *typed = malloc(sizeof(char) * 10);
	strcpy(typed, "");

	int bad_password = 0;

	XEvent event;
	int lock = 1;
	while (lock == 1) {
		XNextEvent(display, &event);

		switch (event.type) {
			case Expose:
				XFillRectangle(display, window, gc_text_white, x + 1, y + 1, 299, 39);
				XDrawString(display, window, gc_text_black, x + 10, y + 25, masked_password, (int) strlen(masked_password));
				if (bad_password == 1) {
					XDrawString(display, window, gc_fore_red, x + 10, y + 25, "Bad password !!!", 16);
				}
				break;
			case KeyPress: {
				KeySym key_sym = XLookupKeysym(&event.xkey, 0);

				switch (key_sym) {
					case XK_Return: {
						if (authenticate(username, password) == 1) {
							lock = 0;
							window_show_all_visible();
						} else {
							bad_password = 1;
							strcpy(password, "");
						}
						break;
					}
					case XK_BackSpace: {
						if (strlen(password) > 0) {
							password[(int) strlen(password) - 1] = '\0';
						}
						break;
					}
					default:
						XmbLookupString(xic, &event.xkey, typed, sizeof(typed), NULL, NULL);
						if (strlen(typed) == 1) {
							strcat(password, typed);
						}
						bad_password = 0;
						break;
				}

				strcpy(masked_password, "");
				for (int i = 0; i < strlen(password); i++) {
					strcat(masked_password, "*");
				}

				XClearArea(display, window, 0, 0, screen_width, screen_height, 1);

				XEvent e = { .type = Expose };
				XSendEvent(display, window, 0, ExposureMask, &e);
				break;
			}
			case MotionNotify:
				if (button_is_hover(event.xmotion.x, event.xmotion.y)) {
					XDefineCursor(display, window, hand_cursor);
				} else {
					XDefineCursor(display, window, cursor);
				}
				break;
			case ButtonPress:
				if (button_is_hover(event.xbutton.x, event.xbutton.y)) {
					if (authenticate(username, password) == 1) {
						lock = 0;
						window_show_all_visible();
					} else {
						bad_password = 1;
						strcpy(password, "");
					}

					strcpy(masked_password, "");
					XClearArea(display, window, 0, 0, screen_width, screen_height, 1);

					XEvent e = { .type = Expose };
					XSendEvent(display, window, 0, ExposureMask, &e);
				}
				break;
		}
	}

	free(typed);
	free(password);
	free(masked_password);

	event_on_expose();
}
