#include <stdio.h>
#include <stdlib.h>
#include "appshortcut.h"

int main(void) {
	int length;
	struct appshortcut *app_shortcuts = appshortcut_get_app_shortcuts(&length);

	for (int i = 0; i < length; i++) {
		printf("%s\n%s -- %s -- %s -- %s\n\n", app_shortcuts[i].file, app_shortcuts[i].name, app_shortcuts[i].exec, app_shortcuts[i].categorie, app_shortcuts[i].icon);
		free(app_shortcuts[i].name);
		free(app_shortcuts[i].exec);
		free(app_shortcuts[i].categorie);
		free(app_shortcuts[i].icon);
		free(app_shortcuts[i].file);
	}

	free(app_shortcuts);
}
