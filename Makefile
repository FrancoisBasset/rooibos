CFLAGS = -Werror -O3
VERSION = $(shell cat VERSION)
ARCH = $(shell uname --machine)
VERSION_LENGTH = $$(( $(shell wc -m < VERSION) - 1 ))
OBJECTS = VERSION.h utils.o appshortcut.o cache.o objects.o window.o icon.o taskbar.o menu.o event.o prompt.o debug.o splash.o sound.o brightness.o battery.o decorator.o lock.o right_click_panel.o wallpaper.o shortcut.o rooibos.o main.o
#WILLDEBUG = -DWILLDEBUG

ifndef WILLDEBUG
	OBJECTS := $(filter-out debug.o,$(OBJECTS))
endif

ifneq "$(SUDO_USER)" "root"
	ROOIBOS_USER = $(SUDO_USER)
endif

ifneq "$(USER)" "root"
	ROOIBOS_USER = $(USER)
endif

rooibos: $(OBJECTS)
	@echo "Compiling executable..."
	@gcc $(CFLAGS) $(WILLDEBUG) *.o -o rooibos -lsqlite3 -lX11 -lgdk_pixbuf-2.0 -lgdk_pixbuf_xlib-2.0 -lasound -lpam
	@echo "Striping executable..."
	@strip rooibos
	@echo "\033[0;32mCompiling success !\033[0m"

VERSION.h:
	@echo "Generating VERSION.h..."
	@xxd -i -len $(VERSION_LENGTH) VERSION > VERSION.h

%.o: %.c
	@echo "Compiling" $<"..."
	@gcc $(CFLAGS) $(WILLDEBUG) -c $< -o $@ -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/lib/glib-2.0/include -I/usr/include/glib-2.0 -I/usr/include/gdk-pixbuf-2.0

clean:
	@echo "Deleting object files..."
	@rm -f *.o

	@echo "Deleting VERSION.h..."
	@rm -f VERSION.h

	@echo "Deleting executable..."
	@rm -f rooibos

	@echo "Deleting package..."
	@rm -rf pkg
	@rm -rf package
	@rm -f *.deb *.zst

	@echo "Deleting debug file..."
	@rm -f debug

	@echo "\033[0;32mCleaning success !\033[0m"

check:
	cppcheck --language=c --enable=all --template=gcc --suppress=missingIncludeSystem .

bear:
	bear -- make

vg:
	valgrind --leak-check=full --show-leak-kinds=all ./rooibos

mat:
	mat2 --inplace ./

exif:
	exiftool -all= -r ./

pkgs:
	mkdir -p \
	package/etc/udev/rules.d \
	package/usr/bin \
	package/usr/share/bash-completion/completions \
	package/usr/share/man/man1 \
	package/usr/share/rooibos \
	package/usr/share/xsessions

	cp rooibos-backlight.rules package/etc/udev/rules.d
	cp rooibos package/usr/bin
	cp rooibos.completion package/usr/share/bash-completion/completions/rooibos
	gzip < rooibos.1 > package/usr/share/man/man1/rooibos.1.gz
	cp assets/* package/usr/share/rooibos
	cp rooibos.desktop package/usr/share/xsessions
	
	if [ -e "/usr/bin/dpkg-deb" ]; then \
		cp -r DEBIAN package/DEBIAN \
	    dpkg-deb --build package rooibos_$(VERSION)_$(ARCH).deb; \
	fi

install:
	cp rooibos /usr/bin/rooibos
	mkdir /usr/share/rooibos
	cp assets/* /usr/share/rooibos
	cp package/usr/share/bash-completion/completions/rooibos /usr/share/bash-completion/completions/rooibos
	cp package/usr/share/xsessions/rooibos.desktop /usr/share/xsessions/rooibos.desktop
	gzip < rooibos.1 > /usr/share/man/man1/rooibos.1.gz
	rooibos cache-init
	rooibos cache-update

	chown -R $(ROOIBOS_USER):$(ROOIBOS_USER) /home/$(ROOIBOS_USER)/.rooibos
	cp package/etc/udev/rules.d/rooibos-backlight.rules /etc/udev/rules.d/rooibos-backlight.rules
	chmod 666 /sys/class/backlight/intel_backlight/brightness

uninstall:
	rm -f /usr/bin/rooibos
	rm -rf /usr/share/rooibos
	rm -f /usr/share/bash-completion/completions/rooibos
	rm -f /usr/share/xsessions/rooibos.desktop
	rm -f /usr/share/man/man1/rooibos.1.gz
	rm -f /etc/udev/rules.d/rooibos-backlight.rules
