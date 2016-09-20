

PREFIX=/usr/local
LIBUSB_INCLUDES=$(shell pkg-config --cflags-only-I libusb-1.0)
LIBUSB_LDFLAGS=$(shell pkg-config --libs libusb-1.0)

CFLAGS=$(LIBUSB_INCLUDES)
LDFLAGS=$(LIBUSB_LDFLAGS) -lfcgi

STATIC=libsirenofshame-static.a
DYNAMIC=libsirenofshame.so
BIN=siren-rest.fcgi
OBJ=sirenofshame.o

all: libs

libs: $(STATIC) $(DYNAMIC)

test: $(OBJ) test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

siren-rest.fcgi: web/fcgi.c $(OBJ)
	$(CC) -I. $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJ): sirenofshame.c sirenofshame.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

$(STATIC): $(OBJ)
	ar rcs $@ $<

$(DYNAMIC): $(OBJ)
	$(CC) $< -shared -o $@

install-all: install install-fcgi

install-fcgi: $(BIN)
	install -D $(BIN) $(PREFIX)/bin/$(BIN)

install: libs
	install -D $(STATIC) $(PREFIX)/lib/$(STATIC)
	install -D $(DYNAMIC) $(PREFIX)/lib/$(DYNAMIC)
	install -D sirenofshame.h $(PREFIX)/include/sirenofshame.h

clean:
	rm -f $(OBJ) $(STATIC) $(DYNAMIC) $(BIN)

.PHONY: libs install-all install install-fcgi
