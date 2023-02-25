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

for_debian() {
	check_installed make /usr/bin/make make
	check_installed pkg-config /usr/bin/pkg-config pkg-config
	check_installed gcc /usr/bin/gcc gcc
	check_installed xorg /usr/bin/startx xorg
	check_installed glibc /usr/include/stdlib.h build-essential
	check_installed libx11-dev /usr/include/X11/Xlib.h libx11-dev
	check_installed libsqlite3-dev /usr/include/sqlite3.h libsqlite3-dev
	check_installed libcairo2-dev /usr/include/cairo/cairo.h libcairo2-dev
	check_installed librsvg2-dev /usr/include/librsvg-2.0/librsvg/rsvg.h librsvg2-dev
	check_installed libjpeg-dev /usr/include/jpeglib.h libjpeg-dev
	check_installed libxpm-dev /usr/include/X11/xpm.h libxpm-dev
	check_installed xterm /usr/bin/xterm xterm

	if [ "$packages" != "" ]
	then
		read -p "apt install $packages [Enter to execute]"
		eval "apt install $packages"
	fi
}

for_arch() {
	check_installed make /usr/bin/make make
	check_installed pkgconf /usr/bin/pkg-config pkgconf
	check_installed gcc /usr/bin/gcc gcc
	check_installed xorg /usr/bin/startx xorg
	check_installed xorg-xinit /usr/bin/startx xorg-xinit
	check_installed glibc /usr/include/stdlib.h glibc
	check_installed libx11-dev /usr/include/X11/Xlib.h libx11-dev
	check_installed libsqlite3-dev /usr/include/sqlite3.h libsqlite3-dev
	check_installed cairo /usr/include/cairo/cairo.h cairo
	check_installed librsvg /usr/include/librsvg-2.0/librsvg/rsvg.h librsvg
	check_installed libjpeg-dev /usr/include/jpeglib.h libjpeg-dev
	check_installed libxpm /usr/include/X11/xpm.h libxpm
	check_installed xterm /usr/bin/xterm xterm

	if [ "$packages" != "" ]
	then
		read -p "pacman -S $packages [Enter to execute]"
		eval "pacman -S $packages"
	fi
}

if [ -f /usr/bin/apt ] 
then
        echo "For Debian and derivatives :"
        for_debian
fi
if [ -f /usr/bin/pacman ]
then
        echo "For Arch and derivatives :"
        for_arch
fi
