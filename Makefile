scum: scum.c
	cc -Wall -std=c99 -pedantic -g -o scum scum.c

clean:
	rm scum
