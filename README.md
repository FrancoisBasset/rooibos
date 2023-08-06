# Rooibos
Rooibos is a modern window manager written in C with the Xlib library, the low-level library of the X Window System. I decided to create this software because I think that I can provide added value.
Rooibos is written with the Xlib library because it is a low-level and lightweight library, and to give the best user experience.

## The Rooibos menu with all apps
![Rooibos menu](/assets/rooibos.png)

## The Rooibos menu with system apps
![Rooibos menu](/assets/rooibos_system.png)

## Rooibos with a terminal
![Rooibos with a terminal](/assets/rooibos_terminal.png)

## Requirements :

These packages are essentials to build the program :
-   `make`
-   `gcc`
-   `build-essential`
-   `libx11-dev`
-   `libsqlite3-dev`
-   `libgdk-pixbuf2.0-dev`
-   `libasound2-dev`

## Installation

The installation of this program is done in 2 steps :
-   Compilation, the process of translating source code by machine code
-   Installation, the process of copying executables and configuration files into a file system

First to compile this program run these commands :

-   `./configure.sh`
-   `make`

Then to install this program there are 2 ways :

-   Classical make install :

    Run this command : `sudo make install` to copy files in /usr/bin and /usr/share folders

-   Debian package install

    Run these 2 commands :
    -   `make pkg` to build the Debian package
    -   `sudo dpkg -i <package.deb>` to install the generated .deb package

## Usability
Window managing is in tilling mode. Window positions is automatic but it can be moved by user.

User need to use the Left Control key and the Super key (Windows key) :
*   A Left Control key press launches a tiny command shell, user is free to type a command. If the command is empty, Rooibos will just launch xterm with no command else a xterm window will be opened and run the given command. Pressing Escape key will quit this shell.
*   A Super key press launches a panel containing :
    *   Desktop apps shortcuts sorted by category
    *   Virtual desktops widget
    *   Date and clock widget
    *   Battery widget
    *   Brightness widget
    *   Volume widget
    *   Lock button
    *   Logout button
    *   Reboot button
    *   Poweroff button

Pressing Super key, Escape key or click outside will quit this panel.

Below there is a task bar with buttons which represents windows. Each window has an icon. If a window is shown then the button is orange else gray.
When a window appears, all windows shown are reorganized.

There are virtual desktops accessible by going beyond the limits of the screen with the pointer.

At the right click in the wallpaper, a window appears, it will serve to :
*   Change wallpaper
*   Add or remove command shortcuts
*   Modify themes

Pressing Escape key or click outside will quit this panel.

To launch Rooibos, you have to execute this command : `startx rooibos`
