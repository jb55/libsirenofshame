

PREFIX=/usr/local
LIBUSB_INCLUDES=$(shell pkg-config --cflags-only-I libusb-1.0)
LIBUSB_LDFLAGS=$(shell pkg-config --libs libusb-1.0)

CFLAGS=$(LIBUSB_INCLUDES) -Ideps
LDFLAGS=$(LIBUSB_LDFLAGS)

STATIC=libsirenofshame-static.a
DEPS=$(wildcard deps/*/*.c)
DYNAMIC=libsirenofshame.so
BIN=siren-rest.cgi
SRCS=sirenofshame.c $(DEPS)
OBJS=$(SRCS:.c=.o)

all: libs

libs: $(STATIC) $(DYNAMIC)

cgi: $(BIN)

test: $(OBJS) test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

siren-rest.cgi: web/cgi.c $(OBJS)
	$(CC) -I. $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

$(STATIC): $(OBJS)
	ar rcs $@ $^

$(DYNAMIC): $(OBJS)
	$(CC) $^ -shared -o $@

install-all: install install-cgi

install-cgi: $(BIN)
	install -D $(BIN) $(PREFIX)/bin/$(BIN)

install: libs
	install -D $(STATIC) $(PREFIX)/lib/$(STATIC)
	install -D $(DYNAMIC) $(PREFIX)/lib/$(DYNAMIC)
	install -D sirenofshame.h $(PREFIX)/include/sirenofshame.h

clean:
	rm -f $(OBJS) $(STATIC) $(DYNAMIC) $(BIN)

.PHONY: libs install-all install install-cgi cgi
