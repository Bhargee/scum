#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

/* All definitions required for datatypes/language constructs and all associated
 * functions for making/modifying/destroying these constructs
 */

typedef enum { FIXNUM } object_t;

typedef struct object
{
    object_t type;
    union 
    {
        struct 
        {
            long value;
        } fixnum;
    } data;
} object;

object*
alloc_object (void)
{
    object *obj;
    obj = malloc (sizeof *obj);
    if (obj == NULL)
    {
        fprintf (stderr, "We've run out of memory!\n");
        exit (1);
    }
    return obj;
}

object*
make_fixnum (long value)
{
    object *obj = alloc_object ();
    obj->type = FIXNUM;
    obj->data.fixnum.value = value;
    return obj;
}

bool 
is_fixnum (object *obj)
{
    return obj->type == FIXNUM;
}

/* All functions required to read from files for interpreting */
bool 
is_delimiter (int c)
{
    return isspace (c) || c == EOF || c == '(' || c == ')' || c == '"'
                    || c == ';';
}

int 
peek (FILE *in)
{
    int value;
    value = getc (in);
    ungetc (value, in);
    return value;
}

void 
rem_whitespace (FILE *in)
{
    int c;

    while ((c = getc (in)) != EOF)
    {
        if (isspace (c))
            continue;
        else if (c == ';')
        {
            while ((c = getc(in)) != EOF && c != '\n');

            continue;
        }
        ungetc (c, in);
        break;
    }
}

object*
read (FILE *in)
{
    int c;
    int sign = 1;
    long num = 0;

    rem_whitespace (in);
    c = getc (in);
    if (isdigit (c) || (c == '-' && isdigit (peek (in))))
    {
        if (c == '-')
            sign = -1;
        else ungetc (c, in);

        while (isdigit ((c = getc (in))))
            num = (num * 10) + (c - '0');
        num *= sign;
        if (is_delimiter (c))
        {
            ungetc (c, in);
            return make_fixnum (num);
        }
        else
        {
            fprintf(stderr, "You need to end a number with a delimiter\n");
            exit (1);
        }
    }
    else
    {
        fprintf(stderr, "Bad input, unexpected %c\n", c);
        exit (1);
    }

    fprintf(stderr, "read illegal state\n");
    exit (1);
}

object*
eval (object *obj)
{
    return obj;
}

void
write (object *obj)
{
    switch (obj->type)
    {
        case FIXNUM:
            printf ("%ld", obj->data.fixnum.value);
            break;
        default:
            fprintf (stderr, "Unknown type\n");
            exit (1);
            break;
    }
}

int 
main()
{
    printf ("Welcome to Scum, the shitty Scheme interpreter!\n");
    while (1)
    {
        printf ("# ");
        write (eval (read (stdin)));
        printf ("\n");
    }
    return 0;
}
