CFLAGS = -Werror -O3
VERSION = $(shell cat VERSION)
ARCH = $(shell uname --machine)
VERSION_LENGTH = $$(( $(shell wc -m < VERSION) - 1 ))

rooibos: utils.o appshortcut.o cache.o VERSION.h main.o
	gcc $(CFLAGS) *.o -o rooibos -lsqlite3
	strip rooibos

VERSION.h:
	xxd -i -len $(VERSION_LENGTH) VERSION > VERSION.h

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f VERSION.h
	rm -f rooibos
	rm -f package/usr/bin/rooibos
	rm -f package/usr/share/man/man1/rooibos.1.gz
	rm -f *.deb

check:
	cppcheck --language=c --enable=all --template=gcc --suppress=missingIncludeSystem .

bear:
	bear -- make

vg:
	valgrind --leak-check=full --show-leak-kinds=all ./rooibos

pkg:
	rm package/usr/bin/.gitkeep
	rm package/usr/share/man/man1/.gitkeep
	cp rooibos package/usr/bin
	gzip < rooibos.1 > package/usr/share/man/man1/rooibos.1.gz
	dpkg-deb --build package rooibos_$(VERSION)_$(ARCH).deb
	> package/usr/bin/.gitkeep
	> package/usr/share/man/man1/.gitkeep

start:
	./rooibos

install:
	cp rooibos /usr/bin/rooibos
	cp package/usr/share/bash-completion/completions/rooibos /usr/share/bash-completion/completions/rooibos
	gzip < rooibos.1 > /usr/share/man/man1/rooibos.1.gz
	cp package/etc/init.d/rooibos.sh /etc/init.d/rooibos.sh
	rooibos cache-init
	rooibos cache-update

uninstall:
	rm -f /usr/bin/rooibos
	rm -f /usr/share/bash-completion/completions/rooibos
	rm -f /usr/share/man/man1/rooibos.1.gz
	rm -f /etc/init.d/rooibos.sh
