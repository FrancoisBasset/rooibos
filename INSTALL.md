# Rooibos Window Manager Installation Guide

## Requirements :

These packages are essentials to build the program :
- `make`
- `pkg-config`
- `gcc`
- `build-essential`
- `libx11-dev`
- `libsqlite3-dev`
- `libgdk-pixbuf2.0-dev`
- `libasound2-dev`

## Installation

The installation of this program is done in 2 steps :
- Compilation, the process of translating source code by machine code
- Installation, the process of copying executables and configuration files into a file system

First to compile this program run these commands :

- `./configure.sh`
- `make`

Then to install this program there are 2 ways :

- Classical make install :

	Run this command : `sudo make install` to copy files in /usr/bin and /usr/share folders

- Debian package install
	
	Run these 2 commands :
	- `make pkg` to build the Debian package
	- `sudo dpkg -i <package.deb>` to install the generated .deb package
