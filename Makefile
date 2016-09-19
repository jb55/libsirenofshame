

PREFIX=/usr/local
CFLAGS=-g

all: test

test: sirenofshame.o test.c
	$(CC) $(CFLAGS) -lusb-1.0 -o $@ $^

sirenofshame.o: sirenofshame.c sirenofshame.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

install: sirenofshame.o
	install -D sirenofshame.o $(PREFIX)/lib/sirenofshame.o

clean:
	rm -f sirenofshame.o
