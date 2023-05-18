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
	char *cache_folder = utils_get(UTILS_FOLDER);
	mkdir(cache_folder, 0700);
	free(cache_folder);

	sqlite3 *db;
	char *cache_path = utils_get(UTILS_CACHE);
	int open_status = sqlite3_open(cache_path, &db);
	free(cache_path);

	if (open_status != SQLITE_OK) {
		sqlite3_close(db);
		return -1;
	}

	const char *create_appshortcuts_request =
		"CREATE TABLE IF NOT EXISTS appshortcuts"
		"('name' TEXT NOT NULL, 'exec' TEXT NOT NULL, 'category' TEXT NOT NULL, 'icon' TEXT NOT NULL, 'file' TEXT NOT NULL)";

	const char *create_cli_history_request = 
		"CREATE TABLE IF NOT EXISTS cli_history"
		"('id' INTEGER NOT NULL, 'command' TEXT NOT NULL, PRIMARY KEY('id' AUTOINCREMENT))";

	int result_stmt1 = cache_init_table(db, create_appshortcuts_request);
	int result_stmt2 = cache_init_table(db, create_cli_history_request);

	sqlite3_close(db);

	if (result_stmt1 == -1 || result_stmt2 == -1) {
		return -1;
	}

	return 0;
}

int cache_init_table(sqlite3 *db, const char *sql) {
	sqlite3_stmt *stmt;
	const char **unused_sql = NULL;
	int prepare_status = sqlite3_prepare_v2(db, sql, (int) strlen(sql) + 1, &stmt, unused_sql);
	if (prepare_status != SQLITE_OK) {
		sqlite3_finalize(stmt);
		return -1;
	}

	int step_status = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	if (step_status != SQLITE_DONE) {
		return -1;
	}

	return 0;
}

int cache_update(void) {
	sqlite3 *db;

	char* cache_path = utils_get(UTILS_CACHE);
	int success = sqlite3_open(cache_path, &db);

	if (success != 0 || access(cache_path, F_OK) == 0) {
		sqlite3_close(db);
		int init_status = cache_init();
		if (init_status == -1) {
			free(cache_path);
			return -1;
		}

		sqlite3_open(cache_path, &db);
	}

	free(cache_path);

	sqlite3_stmt *stmt;
	const char **unused_sql = NULL;

	sqlite3_prepare_v2(db, "SELECT file, exec FROM appshortcuts", 35, &stmt, unused_sql);

	int exists = 0;

	int stmt_status = sqlite3_step(stmt);
	while (stmt_status == 100) {
		const unsigned char *file = sqlite3_column_text(stmt, 0);
		const unsigned char *exec = sqlite3_column_text(stmt, 1);

		exists = cache_exec_exists(exec, file);

		if (access(file, F_OK) != 0 || exists == 0) {
			sqlite3_stmt *delete_stmt;
			sqlite3_prepare_v2(db, "DELETE FROM appshortcuts WHERE file = ?", 50 + (int) strlen(file), &delete_stmt, unused_sql);
			sqlite3_bind_text(delete_stmt, 1, file, (int) strlen(file), SQLITE_STATIC);
			sqlite3_step(delete_stmt);
			sqlite3_finalize(delete_stmt);
		}

		stmt_status = sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);

	int length = 0;
	char **desktop_files = appshortcut_get_all_desktop_files(&length);

	char **all_names = malloc(sizeof(char*) * length);
	int length2 = 0;

	for (int i = 0; i < length; i++) {
		sqlite3_stmt *select_stmt;

		char *short_file = malloc(sizeof(char) * (strlen(desktop_files[i]) + 3));
		strcpy(short_file, desktop_files[i]);

		char *tmp_short_file = malloc(sizeof(char) * (strlen(short_file) + 1));
		strcpy(tmp_short_file, short_file);

		char *tmp = strtok(tmp_short_file, "/");
		while ((tmp = strtok(NULL, "/")) != NULL) {
			strcpy(short_file, "%");
			strcat(short_file, tmp);
			strcat(short_file, "%");
		}

		free(tmp_short_file);

		sqlite3_prepare_v2(db, "SELECT file FROM appshortcuts WHERE file LIKE ?", 50 + (int) strlen(short_file), &select_stmt, unused_sql);
		sqlite3_bind_text(select_stmt, 1, short_file, (int) strlen(short_file), SQLITE_STATIC);
		sqlite3_step(select_stmt);

		free(short_file);

		const unsigned char *file = sqlite3_column_text(select_stmt, 0);
		sqlite3_finalize(select_stmt);

		if (file == NULL) {
			appshortcut_t app_shortcut = appshortcut_get_app_shortcut(desktop_files[i]);
			if (app_shortcut.name != NULL) {
				int has_duplicate = 0;
				for (int j = 0; j < length2; j++) {
					if (strcmp(all_names[j], app_shortcut.name) == 0) {
						has_duplicate = 1;
					}
				}

				all_names[length2] = malloc(sizeof(char) * (strlen(app_shortcut.name) + 1));
				strcpy(all_names[length2], app_shortcut.name);
				length2++;

				exists = cache_exec_exists(app_shortcut.exec, app_shortcut.file);

				if (has_duplicate == 0 && exists == 1) {
					sqlite3_stmt *insert_stmt;
					sqlite3_prepare_v2(db, "INSERT INTO appshortcuts VALUES (?, ?, ?, ?, ?)", 1000, &insert_stmt, unused_sql);
					sqlite3_bind_text(insert_stmt, 1, app_shortcut.name, (int) strlen(app_shortcut.name), SQLITE_STATIC);
					sqlite3_bind_text(insert_stmt, 2, app_shortcut.exec, (int) strlen(app_shortcut.exec), SQLITE_STATIC);
					sqlite3_bind_text(insert_stmt, 3, app_shortcut.category, (int) strlen(app_shortcut.category), SQLITE_STATIC);
					sqlite3_bind_text(insert_stmt, 4, app_shortcut.icon, (int) strlen(app_shortcut.icon), SQLITE_STATIC);
					sqlite3_bind_text(insert_stmt, 5, app_shortcut.file, (int) strlen(app_shortcut.file), SQLITE_STATIC);
					sqlite3_step(insert_stmt);
					sqlite3_finalize(insert_stmt);
				}

				free(app_shortcut.name);
				free(app_shortcut.exec);
				free(app_shortcut.category);
				free(app_shortcut.icon);
				free(app_shortcut.file);
			}
		}

		free(desktop_files[i]);
	}

	for (int i = 0; i < length2; i++) {
		free(all_names[i]);
	}
	free(all_names);
	free(desktop_files);

	sqlite3_close(db);

	return 0;
}

appshortcut_t* cache_get_app_shortcuts(int *length) {
	sqlite3 *db;
	
	char* cache_path = utils_get(UTILS_CACHE);
	int success = sqlite3_open_v2(cache_path, &db, SQLITE_OPEN_READONLY, NULL);

	if (success != 0) {
		sqlite3_close(db);
		int init_status = cache_init();
		if (init_status == -1) {
			free(cache_path);
			return NULL;
		}

		cache_update();
		sqlite3_open_v2(cache_path, &db, SQLITE_OPEN_READONLY, NULL);
	}

	free(cache_path);

	sqlite3_stmt *stmt;
	const char **tail = NULL;

	sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM appshortcuts", 33, &stmt, tail);
	sqlite3_step(stmt);
	*length = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);

	sqlite3_prepare_v2(db, "SELECT * FROM appshortcuts ORDER BY name COLLATE NOCASE", 55, &stmt, tail);

	appshortcut_t *app_shortcuts = malloc(sizeof(appshortcut_t) * *length);
	int stmt_status = sqlite3_step(stmt);
	int i = 0;
	while (stmt_status == 100) {
		const unsigned char *name = sqlite3_column_text(stmt, 0);
		const unsigned char *exec = sqlite3_column_text(stmt, 1);
		const unsigned char *category = sqlite3_column_text(stmt, 2);
		const unsigned char *icon = sqlite3_column_text(stmt, 3);
		const unsigned char *file = sqlite3_column_text(stmt, 4);

		appshortcut_t app_shortcut = { NULL, NULL, NULL, NULL, NULL};

		app_shortcut.name = malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(app_shortcut.name, name);

		app_shortcut.exec = malloc(sizeof(char) * (strlen(exec) + 1));
		strcpy(app_shortcut.exec, exec);

		app_shortcut.category = malloc(sizeof(char) * (strlen(category) + 1));
		strcpy(app_shortcut.category, category);

		app_shortcut.icon = malloc(sizeof(char) * (strlen(icon) + 1));
		strcpy(app_shortcut.icon, icon);

		app_shortcut.file = malloc(sizeof(char) * (strlen(file) + 1));
		strcpy(app_shortcut.file, file);

		app_shortcuts[i] = app_shortcut;

		i++;

		stmt_status = sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return app_shortcuts;
}

char** cache_get_history(int *length) {
	sqlite3 *db;
	char* cache_path = utils_get(UTILS_CACHE);
	int success = sqlite3_open(cache_path, &db);

	if (success != 0) {
		free(cache_path);
		return malloc(sizeof(char*) * 0);
	}

	free(cache_path);

	sqlite3_stmt *select_stmt;
	const char **unused_sql = NULL;

	sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM cli_history", 32, &select_stmt, unused_sql);
	sqlite3_step(select_stmt);
	*length = sqlite3_column_int(select_stmt, 0);
	sqlite3_finalize(select_stmt);

	sqlite3_prepare_v2(db, "SELECT command FROM cli_history", 31, &select_stmt, unused_sql);

	char **history = malloc(sizeof(char*) * *length);

	int stmt_status = sqlite3_step(select_stmt);
	int i = 0;
	while (stmt_status == 100) {
		const unsigned char *command = sqlite3_column_text(select_stmt, 0);

		history[i] = malloc(sizeof(char) * (strlen(command) + 1));
		strcpy(history[i], command);

		i++;

		stmt_status = sqlite3_step(select_stmt);
	}

	sqlite3_finalize(select_stmt);
	sqlite3_close(db);

	return history;
}

void cache_add_history(const char *command) {
	sqlite3 *db;
	char* cache_path = utils_get(UTILS_CACHE);
	int success = sqlite3_open(cache_path, &db);

	if (success != 0) {
		free(cache_path);
		return;
	}

	free(cache_path);

	sqlite3_stmt *insert_stmt;
	const char **unused_sql = NULL;

	sqlite3_prepare_v2(db, "INSERT INTO cli_history('command') VALUES (?)", 1000, &insert_stmt, unused_sql);
	sqlite3_bind_text(insert_stmt, 1, command, (int) strlen(command), SQLITE_STATIC);

	sqlite3_step(insert_stmt);
	sqlite3_finalize(insert_stmt);
	sqlite3_close(db);
}

int cache_exec_exists(const char *exec, const char *file) {
	if (strstr(file, ".local/share") == NULL) {
		return 1;
	}

	char *firstWord = strtok((char*) exec, " ");

	if (access(firstWord, F_OK) == 0) {
		return 1;
	}

	char *path = malloc(sizeof(char) * (strlen(firstWord) + 10));
	sprintf(path, "/usr/bin/%s", firstWord);

	if (access(path, F_OK) == 0) {
		free(path);
		return 1;
	}

	free(path);

	path = malloc(sizeof(char) * (strlen(firstWord) + 12));
	sprintf(path, "/usr/games/%s", firstWord);

	if (access(path, F_OK) == 0) {
		free(path);
		return 1;
	}

	free(path);

	unlink(file);

	return 0;
}