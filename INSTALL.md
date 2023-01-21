# Rooibos Window Manager Installation Guide

The installation of this program is done in 2 steps :
- Compilation, the process of translating source code by machine code
- Installation, the process of copying executables and configuration files into a file system

REQUIREMENTS :
These packages are essentials to build the program :
- make
- gcc
- build-essential
- libx11-dev
- libsqlite3-dev

First to compile this program run these commands :

==> ./configure.sh

==> make

Then to install this program there are 2 ways :

- CLASSICAL MAKE INSTALL :

	Run this command :
	==> sudo make install
	to copy files in /usr/bin and /usr/share folders

OR

- DEBIAN PACKAGE INSTALLATION

	Run these 2 commands :

	Command 1
	==> make pkg
	to build the Debian package

	Command 2
	==> sudo dpkg -i <package.deb>
	to install the generated .deb package
