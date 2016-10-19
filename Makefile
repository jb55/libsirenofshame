

PREFIX=/usr/local
LIBUSB_INCLUDES=$(shell pkg-config --cflags-only-I libusb-1.0)
LIBUSB_LDFLAGS=$(shell pkg-config --libs libusb-1.0)

CFLAGS=$(LIBUSB_INCLUDES) -Ideps
LDFLAGS=$(LIBUSB_LDFLAGS)

STATIC=libsirenofshame-static.a
DEPS=$(wildcard deps/*/*.c)
DYNAMIC=libsirenofshame.so

SRCS=sirenofshame.c $(DEPS)
OBJS=$(SRCS:.c=.o)

all: install-all siren-onoff

libs: $(STATIC) $(DYNAMIC)

cgi: $(BIN)

siren-onoff: $(OBJS) siren-onoff.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

siren-rest.cgi: web/cgi.c $(OBJS)
	$(CC) -I. $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

$(STATIC): $(OBJS)
	ar rcs $@ $^

$(DYNAMIC): $(OBJS)
	$(CC) $^ -shared -o $@

install-all: install install-cgi install-tools

install-cgi: siren-rest.cgi
	install -D siren-rest.cgi $(PREFIX)/bin/siren-rest.cgi

install-tools: siren-onoff
	install -D siren-onoff $(PREFIX)/bin/siren-onoff

install: libs
	install -D siren-onoff.cgi $(PREFIX)/bin/siren-rest.cgi
	install -D $(STATIC) $(PREFIX)/lib/$(STATIC)
	install -D $(DYNAMIC) $(PREFIX)/lib/$(DYNAMIC)
	install -D sirenofshame.h $(PREFIX)/include/sirenofshame.h

clean:
	rm -f $(OBJS) $(STATIC) $(DYNAMIC) $(BIN)

.PHONY: libs install-all install install-cgi cgi
