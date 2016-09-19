

PREFIX=/usr/local
LIBUSB_INCLUDES=$(shell pkg-config --cflags-only-I libusb-1.0)
LIBUSB_LDFLAGS=$(shell pkg-config --libs libusb-1.0)

CFLAGS=$(LIBUSB_INCLUDES)

all: libs

libs: libsirenofshame.a libsirenofshame.so

test: sirenofshame.o test.c
	$(CC) $(CFLAGS) $(LIBUSB_LDFLAGS) -o $@ $^

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
	rm -f sirenofshame.o libsirenofshame.a libsirenofshame.so

.PHONY: libs
