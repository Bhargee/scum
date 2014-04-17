/* 
 * scum - a simple, bare bones, readable scheme interpreter (not garaunteed to
 * be rsr5 compliant, but it does the job)
 */
#include "scum.h"

/* Malloc wrapper for turning tokens into actual objects. Only creates the
 * memory, does not set any values
 * exits if no more memory can be found
 */
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

/* The following functions wrap alloc_object and set the corresponding variables
 * (TYPE and VALUE) to the correct balues
 */
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
   
/* The followng 3 functions implement the cons, car, and cdr list operator for
 * lists and pairs
 */
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

/* checks if a given character (passed in as an integer to accept the EOF) is a
 * character the denotes the boundary between tokens. This is the meat of our
 * pseudo tokenization
 */
bool 
is_delimiter (int c)
{
    return isspace (c) || c == EOF || c == '(' || c == ')' || c == '"'
                    || c == ';';
}

/* checks if a given character is the beginning of a valid symbol, rather than
 * another data type
 */
bool is_symbol_start (int c)
{
    return isalpha(c) || c == '*' || c == '/' || c == '>' ||
             c == '<' || c == '=' || c == '?' || c == '!';
}

/* checks the next character in the input stream without removing it from the
 * stream
 */
int 
peek (FILE *in)
{
    int value;
    value = getc (in);
    ungetc (value, in);
    return value;
}

/* removes whitespace, used to ignore whitespace at the beginning of input and
 * in pairs
 */
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

/* Retursn whether or not the next characters in input match INPUT. If they do,
 * those characters are removed from the stream, and if not, they are put back
 * (the stream doesn't change if the function returns false
 */
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
    
/* Reads a scheme character (delimited with #\), used for tokenization of all
 * character literals
 */
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
    /* This conditional detects the special scheme newline and space characters
     * */
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

/* tokenizes string literals */
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

/* Reads lists and improper lists (pairs) of arbitrary length and composition.
 * Used to tokenize lists, mutually recursive with the read function
 */
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

/* Tokenizer function that calls case specific tokenizers and handles errors */
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

    else if (c == '\'')
        return cons (quote, cons (read (in), nil));

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

/* Evaluator of scheme expressions. Self evaluating atoms are returned as is,
 * while tokens representing operators are applied to arguments 
 */
object*
eval (object *exp, frame *env)
{
    object_t t = exp->type;
    if (t == BOOLEAN || t == FIXNUM || t == CHARACTER || t == STRING)
        return exp;
    else if (has_symbol (quote, exp))
        return cadr (exp);
    else if (has_symbol (define, exp))
        return define_variable (exp, env);
    else if (t == SYMBOL)
        return lookup_variable_value (exp, env);
    else
    {
        fprintf (stderr, "expression has unknown type");
        exit (1);
    }

}

/* checks if a given EXP contains the specified SYMBOL in its car position, used
 * to figure out operator application in scheme
 */
bool
has_symbol (object *symbol, object *exp)
{
    return (exp->type == PAIR && (car (exp))->type == SYMBOL 
            && (car (exp)) == symbol);
}

/* writes back evaluated expressions based on the returned data type */
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

/* Used to wrote improper and proper lists of arbitrary length and composition,
 * mutually recursove with write*/
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
/* creates all the global objects (the boolean literals for true and false, the
 * empty list, and operator symbols) so we don't waste memory creates new copies
 * of objects that represent keywords
 */
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

    quote = make_symbol ("quote");
    define = make_symbol ("define");
    set = make_symbol ("set!");
    ok = make_symbol ("ok");
}


/* The following functions are used in the symbol table, a chained hash map of
 * symbols
 */
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
    symbol_table_entry *e = lookup (value);
    if (e != NULL && strcmp(e->object->data.symbol.value, value) == 0)
        return e->object;
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

void
interpret(FILE *in, bool silent)
{
    int instr_count = 1;
    make_singletons ();
    setup_env();
    if (!silent)
        printf ("Welcome to Scum, the shitty Scheme interpreter!\n");
    while (1)
    {
        if (!silent)
        {
            printf ("%d> ", instr_count++);
            write (eval (read (in), curr_frame));
            printf ("\n");
        }
        else
        {
            eval (read (in), curr_frame);
        }
    }
    fclose (in);
}

binding*
make_binding (object *var, object *val)
{
    binding *b;
    b = malloc (sizeof *b);
    if (b == NULL)
    {
        fprintf (stderr, "Not enough memory\n");
        exit (1);
    }
    b->var = var;
    b->val = val;
    b->next = NULL;
    return b;
}

frame*
make_frame (binding *binding)
{
    frame *f;
    f = malloc (sizeof *f);
    if (f == NULL)
    {
        fprintf (stderr, "Not enough memory\n");
        exit (1);
    }
    f->bindings = binding;
    f->enclosing_env = NULL;
    return f;
}

void
setup_env (void)
{
    curr_frame = global_frame;
}

void
add_binding (binding *new_binding, frame *f)
{
    binding *prev_bindings = f->bindings;
    while (prev_bindings->next != NULL)
        prev_bindings = prev_bindings->next;
    prev_bindings->next = new_binding;
}

object*
define_variable (object *exp, frame *env)
{
    object *def_var = cadr (exp);
    object *def_val = eval (caddr (exp), env);
    binding *b = make_binding (def_var, def_val);
    if (curr_frame == NULL)
        curr_frame = make_frame (b);
    else
        add_binding (b, env);

    return ok;

}

object*
lookup_variable_value (object *exp, frame *env)
{
   frame *curr = env;
   while ( curr != NULL)
   {
       binding *b = curr->bindings;
       while (b != NULL)
       {
           if (strcmp (exp->data.symbol.value, b->var->data.symbol.value) == 0)
               return b->val;
           b = b->next;
       }
       curr = curr->enclosing_env;
   }
   fprintf(stderr, "unbound variable\n");
   exit(1);
}
