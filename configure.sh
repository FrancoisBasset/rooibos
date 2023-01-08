green="\e[32m"
red="\e[31m"
normal="\e[0m"

check_installed() {
	if [ -f $2 ]
	then
		echo -e "${green}$1 is installed $normal"
	else
		echo -e "${red}Please install $3 $normal"
	fi
}

check_installed make /usr/bin/make make
check_installed gcc /usr/bin/gcc gcc
check_installed xorg /usr/bin/startx xorg
check_installed stdlib /usr/include/stdlib.h build-essential
check_installed libsqlite3-dev /usr/include/sqlite3.h libsqlite3-dev
check_installed xterm /usr/bin/xterm xterm
