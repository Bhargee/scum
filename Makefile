scum: scum.c
	cc -Wall -std=c99 -pedantic -ggdb -o scum scum.c

clean:
	rm scum
