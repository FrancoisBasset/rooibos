#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sqlite3.h>
#include "cache.h"
#include "appshortcut.h"
#include "utils.h"

int cache_init(void) {
	char *user_home = utils_get_user_home();
	char *cache_path = malloc(sizeof(char) * (strlen(user_home) + 19));

	strcpy(cache_path, user_home);
	strcat(cache_path, "/.rooibos");

	if (access(cache_path, F_OK) != 0) {
		mkdir(cache_path, 0700);
	}

	strcat(cache_path, "/cache.db");

	sqlite3 *db;
	int open_status = sqlite3_open(cache_path, &db);

	free(cache_path);

	if (open_status != SQLITE_OK) {
		sqlite3_close(db);
		return -1;
	}

	sqlite3_stmt *stmt;
	char *sql = "CREATE TABLE IF NOT EXISTS appshortcuts ('name' TEXT NOT NULL, 'exec' TEXT NOT NULL, 'categorie' TEXT NOT NULL, 'icon' TEXT NOT NULL, 'file' TEXT NOT NULL)";
	const char **unused_sql;
	int prepare_status = sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &stmt, unused_sql);
	if (prepare_status != SQLITE_OK) {
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return -1;
	}

	int step_status = sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	if (step_status != SQLITE_DONE) {
		return -1;
	}

	return 0;
}

int cache_update(void) {
	char *user_home = utils_get_user_home();
	char *cache_path = malloc(sizeof(char) * (strlen(user_home) + 19));

	strcpy(cache_path, user_home);
	strcat(cache_path, "/.rooibos/cache.db");

	sqlite3 *db;
	int success = sqlite3_open(cache_path, &db);
	free(cache_path);

	if (success != 0) {
		printf("Cannot get cache !\n");
		printf("Trying to recreate cache !\n");
		sqlite3_close(db);
		return -1;
	}

	sqlite3_stmt *stmt;
	const char **unused_sql;

	sqlite3_prepare_v2(db, "DELETE FROM appshortcuts", 24, &stmt, unused_sql);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	int length = 0;
	struct appshortcut *app_shortcuts = appshortcut_get_app_shortcuts(&length);

	for (int i = 0; i < length; i++) {
		sqlite3_prepare_v2(db, "INSERT INTO appshortcuts VALUES (?, ?, ?, ?, ?)", 1000, &stmt, unused_sql);
		sqlite3_bind_text(stmt, 1, app_shortcuts[i].name, strlen(app_shortcuts[i].name), SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, app_shortcuts[i].exec, strlen(app_shortcuts[i].exec), SQLITE_STATIC);
		sqlite3_bind_text(stmt, 3, app_shortcuts[i].categorie, strlen(app_shortcuts[i].categorie), SQLITE_STATIC);
		sqlite3_bind_text(stmt, 4, app_shortcuts[i].icon, strlen(app_shortcuts[i].icon), SQLITE_STATIC);
		sqlite3_bind_text(stmt, 5, app_shortcuts[i].file, strlen(app_shortcuts[i].file), SQLITE_STATIC);

		sqlite3_step(stmt);

		free(app_shortcuts[i].name);
		free(app_shortcuts[i].exec);
		free(app_shortcuts[i].categorie);
		free(app_shortcuts[i].icon);
		free(app_shortcuts[i].file);

		sqlite3_finalize(stmt);
	}

	free(app_shortcuts);

	sqlite3_close(db);

	return 0;
}

struct appshortcut* cache_get_app_shortcuts(int *length) {
	char *user_home = utils_get_user_home();
	char *cache_path = malloc(sizeof(char) * (strlen(user_home) + 19));

	strcpy(cache_path, user_home);
	strcat(cache_path, "/.rooibos/cache.db");

	sqlite3 *db;
	int success = sqlite3_open_v2(cache_path, &db, SQLITE_OPEN_READONLY, NULL);
	free(cache_path);

	if (success != 0) {
		printf("Cannot get cache !\n");
		printf("Trying to recreate cache !\n");
		sqlite3_close(db);
		return NULL;
	}

	sqlite3_stmt *stmt;
	const char **tail;

	sqlite3_prepare(db, "SELECT COUNT(*) FROM appshortcuts", 33, &stmt, tail);
	sqlite3_step(stmt);
	*length = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);

	sqlite3_prepare(db, "SELECT * FROM appshortcuts ORDER BY name COLLATE NOCASE", 55, &stmt, tail);

	struct appshortcut *app_shortcuts = malloc(sizeof(struct appshortcut) * *length);
	int stmt_status;
	int i = 0;
	while ((stmt_status = sqlite3_step(stmt)) == 100) {
		const unsigned char *name = sqlite3_column_text(stmt, 0);
		const unsigned char *exec = sqlite3_column_text(stmt, 1);
		const unsigned char *categorie = sqlite3_column_text(stmt, 2);
		const unsigned char *icon = sqlite3_column_text(stmt, 3);
		const unsigned char *file = sqlite3_column_text(stmt, 4);

		struct appshortcut app_shortcut = { NULL, NULL, NULL, NULL, NULL};

		app_shortcut.name = malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(app_shortcut.name, name);

		app_shortcut.exec = malloc(sizeof(char) * (strlen(exec) + 1));
		strcpy(app_shortcut.exec, exec);

		app_shortcut.categorie = malloc(sizeof(char) * (strlen(categorie) + 1));
		strcpy(app_shortcut.categorie, categorie);

		app_shortcut.icon = malloc(sizeof(char) * (strlen(icon) + 1));
		strcpy(app_shortcut.icon, icon);

		app_shortcut.file = malloc(sizeof(char) * (strlen(file) + 1));
		strcpy(app_shortcut.file, file);

		app_shortcuts[i] = app_shortcut;

		i++;
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return app_shortcuts;
}
