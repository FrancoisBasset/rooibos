CFLAGS = -Werror -O3 -g

rooibos: utils.o appshortcut.o cache.o main.o
	gcc $(CFLAGS) *.o -o rooibos -lsqlite3

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f rooibos

check:
	cppcheck --language=c --enable=all --template=gcc --suppress=missingIncludeSystem .

vg:
	valgrind --leak-check=full --show-leak-kinds=all ./rooibos

pkg:
	ls

start:
	./rooibos

install:
	cp ./rooibos /usr/bin/rooibos

uninstall:
	rm -r /usr/bin/rooibos
