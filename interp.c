#include <stdio.h>
#include "scum.h"

int 
main (int argc, char **argv)
{
    FILE *f = stdin;
    if (argc > 1)
        f = fopen (argv[1], "r");
    interpret (f, false);
    fclose (f);
}
