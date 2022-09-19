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

	sqlite3_prepare_v2(db, "SELECT file FROM appshortcuts", 29, &stmt, unused_sql);

	int stmt_status;
	while ((stmt_status = sqlite3_step(stmt)) == 100) {
		const unsigned char *file = sqlite3_column_text(stmt, 0);

		if (access(file, F_OK) != 0) {
			sqlite3_stmt *delete_stmt;
			sqlite3_prepare_v2(db, "DELETE FROM appshortcuts WHERE file = ?", 50 + strlen(file), &delete_stmt, unused_sql);
			sqlite3_bind_text(delete_stmt, 1, file, strlen(file), SQLITE_STATIC);
			sqlite3_step(delete_stmt);
			sqlite3_finalize(delete_stmt);
		}
	}

	sqlite3_finalize(stmt);

	int length = 0;
	char **desktop_files = appshortcut_get_all_desktop_files(&length);

	for (int i = 0; i < length; i++) {
		sqlite3_stmt *select_stmt;
		sqlite3_prepare_v2(db, "SELECT file FROM appshortcuts WHERE file = ?", 50 + strlen(desktop_files[i]), &select_stmt, unused_sql);
		sqlite3_bind_text(select_stmt, 1, desktop_files[i], strlen(desktop_files[i]), SQLITE_STATIC);
		sqlite3_step(select_stmt);

		const unsigned char *file = sqlite3_column_text(select_stmt, 0);
		sqlite3_finalize(select_stmt);

		if (file == NULL) {
			struct appshortcut app_shortcut = appshortcut_get_app_shortcut(desktop_files[i]);
			if (app_shortcut.name != NULL) {
				sqlite3_stmt *insert_stmt;
				sqlite3_prepare_v2(db, "INSERT INTO appshortcuts VALUES (?, ?, ?, ?, ?)", 1000, &insert_stmt, unused_sql);
				sqlite3_bind_text(insert_stmt, 1, app_shortcut.name, strlen(app_shortcut.name), SQLITE_STATIC);
				sqlite3_bind_text(insert_stmt, 2, app_shortcut.exec, strlen(app_shortcut.exec), SQLITE_STATIC);
				sqlite3_bind_text(insert_stmt, 3, app_shortcut.categorie, strlen(app_shortcut.categorie), SQLITE_STATIC);
				sqlite3_bind_text(insert_stmt, 4, app_shortcut.icon, strlen(app_shortcut.icon), SQLITE_STATIC);
				sqlite3_bind_text(insert_stmt, 5, app_shortcut.file, strlen(app_shortcut.file), SQLITE_STATIC);
				sqlite3_step(insert_stmt);
				sqlite3_finalize(insert_stmt);

				free(app_shortcut.name);
				free(app_shortcut.exec);
				free(app_shortcut.categorie);
				free(app_shortcut.icon);
				free(app_shortcut.file);
			}
		}

		free(desktop_files[i]);
	}

	free(desktop_files);

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

	sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM appshortcuts", 33, &stmt, tail);
	sqlite3_step(stmt);
	*length = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);

	sqlite3_prepare_v2(db, "SELECT * FROM appshortcuts ORDER BY name COLLATE NOCASE", 55, &stmt, tail);

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
