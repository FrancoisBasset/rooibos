#!/bin/bash

green="\e[32m"
red="\e[31m"
normal="\e[0m"

packages=""

check_installed() {
	if [ -f "$2" ]
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
	check_installed libgdk-pixbuf2.0-dev /usr/include/gdk-pixbuf-2.0/gdk-pixbuf/gdk-pixbuf.h libgdk-pixbuf2.0-dev
	check_installed libasound2-dev /usr/include/alsa/asoundlib.h libasound2-dev
	check_installed xterm /usr/bin/xterm xterm

	if [ "$packages" != "" ]
	then
		read -p "sudo apt install $packages [Enter to execute]"
		eval "sudo apt install $packages"
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
	check_installed gdk-pixbuf2 /usr/include/gdk-pixbuf-2.0/gdk-pixbuf/gdk-pixbuf.h gdk-pixbuf2
	check_installed alsa-lib /usr/include/alsa/asoundlib.h alsa-lib
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
