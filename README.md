# Rooibos
Rooibos is a window manager written in C with the XCB library, the low-level library of the X Window System. I decided to create this software because I think that I can provide added value.
Rooibos is written with the XCB library because it is a low-level and lightweight library, and to give the best user experience.

## Usability
Window managing is in tilling mode. Window positions is automatic but it can be moved by user.

User need to use the Meta key (Windows key) :
* A short press launches a tiny command shell, user is free to type a command. If the command is empty, Rooibos will just launch xterm with no command, else a xterm window will be opened and run the given command. Pressing Escape key will quit this shell.
* A long press launches a panel containing :
  * Desktop apps shortcuts sorted by category
  * Virtual desktops widget
  * Date and clock widget
  * Battery widget
  * Brightness widget
  * Volume widget
  * Lock button
  * Logout button
  * Reboot button
  * Poweroff button

Pressing Escape key or click outside will quit this panel.

Below there is a task bar with buttons which represents windows. Each window has an icon. If a window is shown then the button is green else red.
When a window appears, all windows shown are reorganized.

There are virtual desktops accessible by going beyond the limits of the screen with the pointer.

At the right click in the wallpaper, a window appears, it will serve to :
* Change wallpaper
* Add or remove command shortcuts
* Modify themes

Pressing Escape key or click outside will quit this panel.
