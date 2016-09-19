

PREFIX=/usr/local
CFLAGS=-g

all: test

test: sirenofshame.o test.c
	$(CC) $(CFLAGS) -lusb-1.0 -o $@ $^

sirenofshame.o: sirenofshame.c sirenofshame.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

libsirenofshame.a: sirenofshame.o
	ar rcs $@ $<

libsirenofshame.so: sirenofshame.o
	$(CC) $< -shared -o $@

install: libsirenofshame.a libsirenofshame.so
	install -D libsirenofshame.a $(PREFIX)/lib/libsirenofshame-static.a
	install -D libsirenofshame.so $(PREFIX)/lib/libsirenofshame.so
	install -D sirenofshame.h $(PREFIX)/include/sirenofshame.h

clean:
	rm -f sirenofshame.o
