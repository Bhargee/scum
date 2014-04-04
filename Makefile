scum: scum.c
	cc -Wall -std=c99 -pedantic -o scum scum.c

clean:
	rm scum
