CFLAGS = -Werror -O3 -g

appshortcuts: utils.o appshortcut.o cache.o main.o
	gcc $(CFLAGS) *.o -o appshortcuts -lsqlite3

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f appshortcuts

check:
	cppcheck --language=c --enable=all --template=gcc --suppress=missingIncludeSystem .

vg:
	valgrind --leak-check=full --show-leak-kinds=all ./appshortcuts

pkg:
	ls

start:
	./appshortcuts

install:
	cp ./appshortcuts /usr/bin/appshortcuts

uninstall:
	rm -r /usr/bin/appshortcuts
