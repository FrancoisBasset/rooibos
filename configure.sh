green="\e[32m"
red="\e[31m"
yellow="\e[33m"
normal="\e[0m"

packages=""

check_installed() {
	if [ -f $2 ]
	then
		echo -e "${green}$1 is installed $normal"
	else
		echo -e "${red}Please install $3 $normal"
		packages+="$3 "
	fi
}

check_installed make /usr/bin/make make
check_installed gcc /usr/bin/gcc gcc
check_installed xorg /usr/bin/startx xorg
check_installed stdlib /usr/include/stdlib.h build-essential
check_installed libx11-dev /usr/include/X11/Xlib.h libx11-dev
check_installed libsqlite3-dev /usr/include/sqlite3.h libsqlite3-dev
check_installed libcairo2-dev /usr/include/cairo/cairo.h libcairo2-dev
check_installed librsvg2-dev /usr/include/librsvg-2.0/librsvg/rsvg.h librsvg2-dev
check_installed libjpeg62-turbo-dev /usr/include/jpeglib.h libjpeg62-turbo-dev
check_installed xterm /usr/bin/xterm xterm

if [ "$packages" != "" ]
then
	echo -e "${yellow}apt install $packages${normal}"
fi