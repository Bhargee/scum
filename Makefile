CFLAGS=-Wall -std=c99 -pedantic -g

all: scum check

check: check_scum.c scum.o
	cc $(CFLAGS) -o check_scum check_scum.c scum.o -lcheck
	./check_scum

scum: interp.c scum.o
	cc $(CFLAGS) -o scum interp.c scum.o

scum.o: scum.c
	cc $(CFLAGS) -c scum.c

clean:
	rm scum.o
	rm scum
	rm check_scum
	rm -r *.dSYM
