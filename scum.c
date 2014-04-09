#include "scum.h"

object*
alloc_object (void)
{
    object *obj;
    obj = (object *)malloc (sizeof *obj);
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
    if (value)
        return t;
    return f;
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
   
object*
cons (object *car, object *cdr)
{
    object *obj = alloc_object ();
    obj->type = PAIR;
    obj->data.pair.car = car;
    obj->data.pair.cdr = cdr;
    return obj;
}

object*
car (object *pair)
{
    if (pair->type != PAIR)
    {
        fprintf (stderr, "Object is not a list\n");
        exit (1);
    }
    return pair->data.pair.car;
}

object*
cdr (object *pair)
{
    if (pair->type != PAIR)
    {
        fprintf (stderr, "Object is not a list\n");
        exit (1);
    }
    return pair->data.pair.cdr;
}

bool 
is_delimiter (int c)
{
    return isspace (c) || c == EOF || c == '(' || c == ')' || c == '"'
                    || c == ';';
}

bool is_symbol_start (int c)
{
    return isalpha(c) || c == '*' || c == '/' || c == '>' ||
             c == '<' || c == '=' || c == '?' || c == '!';
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
    while ((c = getc (in)) != EOF && i < MAX_STRING_LEN)
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
            else if (c == 'a')
                c = '\a';
        }
        buf[i++] = c;
    }
    fprintf (stderr, "Reached EOF or exceeded string max limit\n");
    exit (1);
}

object*
read_pair (FILE* in)
{
    int c;
    object *car;
    object *cdr;

    rem_whitespace (in);
    
    if ((c = getc (in)) == ')')
        return nil;
    ungetc (c, in);
    car = read (in);

    rem_whitespace (in);
    c = getc (in);
    if (c == '.')
    {
        if (!is_delimiter (peek (in)))
        {
            fprintf (stderr, "need a delimiter after dot op\n");
            exit (1);
        }
        cdr = read (in);
        rem_whitespace (in);
        if ((c = getc (in)) != ')')
        {
            fprintf (stderr, "unmatched parenthesis\n");
            exit (1);
        }
        return cons (car, cdr);
    }
        
    ungetc (c, in);
    cdr = read_pair (in);
    return cons (car, cdr);
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
            return t;
        else if (c == 'f')
            return f;
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

    else if (c == '(')
    {
        return read_pair (in);
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
    else if (is_symbol_start (c) || ((c == '+' || c == '-') 
             && is_delimiter (peek (in))))
    {
        int i = 1;
        char buf[MAX_STRING_LEN];
        buf[0] = c;
        while ((c = getc (in)) != EOF && (is_symbol_start (c) 
                || isdigit (c) || c == '+' || c == '-'))
        {
            if (i < MAX_STRING_LEN - 1)
                buf[i++] = c;
            else
            {
                fprintf (stderr, "symbol too long\n");
                exit (1);
            }
        }
        if (is_delimiter (c))
        {
            buf[i] = '\0';
            ungetc (c, in);
            return make_symbol (buf);
        }
        else
        {
            fprintf (stderr, "need to end symbol with delimiter\n");
            exit (1);
        }
    }
    else if (c == EOF)
        exit (0);
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
            printf("\"%s\"", obj->data.string.value);
            break;
        case NIL:
            printf("()");
            break;
        case PAIR:
            printf ("(");
            write_pair (obj);
            printf (")");
            break;
        case SYMBOL:
            printf ("%s", obj->data.symbol.value);
            break;
        default:
            fprintf (stderr, "Unknown type\n");
            exit (1);
            break;
    }
}

void
write_pair (object* pair)
{
    object *cdr;

    write(pair->data.pair.car);
    cdr = pair->data.pair.cdr;
    if (cdr->type == PAIR) {
        printf(" ");
        write_pair(cdr);
    }
    else if (cdr->type == NIL)
        return;
    else {
        printf(" . ");
        write(cdr);
    }
}

void
make_singletons (void)
{
    t = alloc_object();
    t->type = BOOLEAN;
    t->data.boolean.value = true;
    f = alloc_object();
    f->type = BOOLEAN;
    f->data.boolean.value = false;

    nil = alloc_object();
    nil->type = NIL;
}

unsigned
hash (char *s)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % SYMBOL_TABLE_LEN;
}

symbol_table_entry*
lookup (char *val)
{
    symbol_table_entry *e = symbol_table[hash(val)];
    while (e != NULL)
    {
        if (strcmp(e->object->data.symbol.value, val) == 0)
            return e;
        e = e->next;
    }
    return NULL;
}

symbol_table_entry*
install (object *obj)
{
    unsigned hashval;
    symbol_table_entry *e = lookup (obj->data.symbol.value);
    if (e == NULL)
    {
        e = (symbol_table_entry *)malloc (sizeof (symbol_table_entry));
        if (e == NULL)
            return NULL;
        e->object = obj;
        hashval = hash (obj->data.symbol.value);
        e->next = symbol_table[hashval];
        symbol_table[hashval] = e;
    }
    else
        free (e->object);
    e->object = obj;
    return e;
}

object* 
make_symbol (char *value)
{
    object *obj = alloc_object ();
    obj->type = SYMBOL;
    obj->data.symbol.value = (char*) malloc(strlen(value)+1);
    if (obj->data.symbol.value == NULL)
    {
        fprintf (stderr, "no more memory");
        exit (1);
    }
    strcpy (obj->data.symbol.value, value);
    install (obj);
    return obj;
}

int 
main(int argc, char **argv)
{
    FILE *file = stdin;
    int instr_count = 1;
    if (argc > 1)
    {
        file = fopen (argv[1], "r");
    }
    
    make_singletons ();
    printf ("Welcome to Scum, the shitty Scheme interpreter!\n");
    while (1)
    {
        printf ("%d> ", instr_count++);
        write (eval (read (file)));
        printf ("\n");
    }
    fclose (file);
    return 0;
}
