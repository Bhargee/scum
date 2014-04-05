#include "scum.h"

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

object*
make_boolean (bool value)
{
    object *obj = alloc_object ();
    obj->type = BOOLEAN;
    obj->data.boolean.value = value;
    return obj;
} 

object*
make_character (char value)
{
    object *obj = alloc_object ();
    obj->type = CHARACTER;
    obj->data.character.value = value;
    return obj;
}

object*
make_string (char* value)
{
    object *obj = alloc_object ();
    obj->type = STRING;
    obj->data.string.value = (char*) malloc(strlen(value)+1);
    if (obj->data.string.value == NULL)
    {
        fprintf (stderr, "no more memory");
        exit (1);
    }
    strcpy (obj->data.string.value, value);
    return obj;
}

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

bool
is_next_input (FILE *in, char *input)
{
   int input_len = strlen (input);
   char buf[input_len];
   int i = 0;

   while ((buf[i] = getc (in)) != EOF && *input != '\0')
   {
       if(buf[i] != *input)
       {
           break;
       }
       i++;
       input++;
   }
   if(*input == '\0')
       return true;
   while(i >= 0)
   {
       ungetc (buf[i], in);
       i--;
   }
   return false;
}
    
char
read_character (FILE *in)
{
    int c;
    c = getc (in);
    if (c == EOF || c == '\n')
    {
        fprintf (stderr, "Premature EOF\n");
        exit (1);
    }
    if (c == 's' && is_next_input (in, "pace"))
        return ' ';
    else if (c == 'n' && is_next_input (in, "ewline"))
        return '\n';

    if (!is_delimiter (peek (in)))
    {
        fprintf (stderr, "No delimiter\n");
        exit (1);
    }
    return c;
}

void
read_string (FILE* in, char* buf)
{
    int c;
    int i = 0;
    while ((c = getc (in)) != EOF)
    {
       if (c == '"')
        {
            buf[i++] = '\0';
            return;
        }

       if (c == '\\')
        {
            c = getc (in);
            if (c == 'n')
                c = '\n';
            else if (c == '"')
                c = '"';
            else if (c == 't')
                c = '\t';
            else if (c == '\\')
                c = '\\';
        }
        buf[i++] = c;
    }
    fprintf (stderr, "Reached EOF %c\n", c);
    exit (1);
}

object*
read (FILE *in)
{
    int c;
    int sign = 1;
    long num = 0;

    rem_whitespace (in);
    c = getc (in);
    
    if (c == '#')
    {
        c = getc (in);
        if (c == 't')
            return make_boolean (true);
        else if (c == 'f')
            return make_boolean (false);
        else if (c == '\\')
            return make_character (read_character (in));
        else
        {
            fprintf (stderr, "Unknown boolean literal %c\n", c);
            exit (1);
        }
    }

    else if (c == '"')
    {
        char buf[MAX_STRING_LEN];
        read_string (in, buf);
        return make_string (buf);
    }

    else if (isdigit (c) || (c == '-' && isdigit (peek (in))))
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
        fprintf (stderr, "Bad input, unexpected %c\n", c);
        exit (1);
    }

    fprintf (stderr, "read illegal state\n");
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
        case BOOLEAN:
            if (obj->data.boolean.value)
                printf("#t");
            else
                printf("#f");
            break;
        case CHARACTER:
            printf("%c", obj->data.character.value);
            break;
        case STRING:
            printf("%s", obj->data.string.value);
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
        printf ("> ");
        write (eval (read (stdin)));
        printf ("\n");
    }
    return 0;
}
