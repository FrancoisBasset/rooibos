CFLAGS = -Werror -O3
VERSION = $(shell cat VERSION)
ARCH = $(shell uname --machine)
VERSION_LENGTH = $$(( $(shell wc -m < VERSION) - 1 ))
OBJECTS = utils.o appshortcut.o cache.o VERSION.h objects.o window.o icon.o taskbar.o toolbar.o menu.o event.o prompt.o debug.o splash.o cairo_jpg.o rooibos.o main.o
#WILLDEBUG = -DWILLDEBUG

ifndef WILLDEBUG
	OBJECTS := $(filter-out debug.o,$(OBJECTS))
endif

rooibos: $(OBJECTS)
	gcc $(CFLAGS) $(WILLDEBUG) *.o -o rooibos -lsqlite3 -lX11 -lcairo `pkg-config --cflags --libs librsvg-2.0` -ljpeg
	strip rooibos

VERSION.h:
	xxd -i -len $(VERSION_LENGTH) VERSION > VERSION.h

%.o: %.c
	gcc $(CFLAGS) $(WILLDEBUG) -c $< -o $@ `pkg-config --cflags --libs librsvg-2.0`

clean:
	rm -f *.o
	rm -f VERSION.h
	rm -f rooibos
	rm -f package/usr/bin/rooibos
	rm -f package/usr/share/man/man1/rooibos.1.gz
	rm -f *.deb
	rm -f debug

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

pkg:
	rm -f package/usr/bin/.gitkeep
	rm -f package/usr/share/rooibos/.gitkeep
	rm -f package/usr/share/man/man1/.gitkeep
	cp rooibos package/usr/bin
	cp assets/logo.svg package/usr/share/rooibos
	cp assets/wallpaper.jpg package/usr/share/rooibos
	gzip < rooibos.1 > package/usr/share/man/man1/rooibos.1.gz
	dpkg-deb --build package rooibos_$(VERSION)_$(ARCH).deb
	> package/usr/bin/.gitkeep
	> package/usr/share/rooibos/.gitkeep
	> package/usr/share/man/man1/.gitkeep

start:
	./rooibos

install:
	cp rooibos /usr/bin/rooibos
	mkdir /usr/share/rooibos
	cp assets/logo.svg /usr/share/rooibos
	cp assets/wallpaper.jpg /usr/share/rooibos
	cp package/usr/share/bash-completion/completions/rooibos /usr/share/bash-completion/completions/rooibos
	cp package/usr/share/xsessions/rooibos.desktop /usr/share/xsessions/rooibos.desktop
	gzip < rooibos.1 > /usr/share/man/man1/rooibos.1.gz
	rooibos cache-init
	rooibos cache-update

uninstall:
	rm -f /usr/bin/rooibos
	rm -rf /usr/share/rooibos
	rm -f /usr/share/bash-completion/completions/rooibos
	rm -r /usr/share/xsessions/rooibos.desktop
	rm -f /usr/share/man/man1/rooibos.1.gz
