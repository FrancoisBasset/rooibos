#include <pwd.h>
#include <unistd.h>

struct passwd *pw = NULL;

char* utils_get_login(void) {
	if (pw == NULL) {
		pw = getpwnam(getlogin());
	}

	return pw->pw_name;
}

char* utils_get_user_home(void) {
	if (pw == NULL) {
		pw = getpwnam(getlogin());
	}

	return pw->pw_dir;
}
