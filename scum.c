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

object*
make_primitive_proc (object *(*fun)(struct object *arguments))
{
    object *obj;

    obj = alloc_object();
    obj->type = PRIM_PROC;
    obj->data.prim_proc.fun = fun;
    return obj;
}

object*
make_compound_proc (object *params, object *body, object *env)
{
    object *obj = alloc_object ();
    obj->type = COMPOUND_PROC;
    obj->data.compound_proc.parameters = params;
    obj->data.compound_proc.body = body;
    obj->data.compound_proc.env = env;
    return obj;
}

/*
 * The following are all standard library functions meant to be called from
 * within the interpreter. They are bound o symbols that, when evaluated, cause
 * these functions to run
 */

object*
is_null_proc (object *arguments)
{
    return (car(arguments))->type == NIL ? t : f;
}

object* 
is_boolean_proc (object *arguments)
{
    return (car(arguments))->type == BOOLEAN ? t : f;
}

object* 
is_symbol_proc (object *arguments)
{
    return (car(arguments))->type == SYMBOL ? t : f;
}

object* 
is_integer_proc (object *arguments)
{
    return (car(arguments))->type == FIXNUM ? t : f;
}

object*
is_char_proc (object *arguments)
{
    return (car(arguments))->type == CHARACTER ? t : f;
}

object*
is_string_proc (object *arguments)
{
    return (car(arguments))->type == STRING ? t : f;
}

object*
is_pair_proc (object *arguments)
{
    return (car(arguments))->type == PAIR ? t : f;
}

object*
is_procedure_proc (object *arguments)
{
    return (car(arguments))->type == PRIM_PROC ? t : f;
}

object*
char_to_integer_proc (object *arguments)
{
    return make_fixnum((car(arguments))->data.character.value);
}

object*
integer_to_char_proc (object *arguments)
{
    return make_character((car(arguments))->data.fixnum.value);
}

object*
number_to_string_proc (object *arguments)
{
    char buffer[100];

    sprintf(buffer, "%ld", (car(arguments))->data.fixnum.value);
    return make_string(buffer);
}

object*
string_to_number_proc (object *arguments)
{
    return make_fixnum(atoi((car(arguments))->data.string.value));
}

object*
symbol_to_string_proc (object *arguments)
{
    return make_string((car(arguments))->data.symbol.value);
}

object*
string_to_symbol_proc (object *arguments)
{
    return make_symbol((car(arguments))->data.string.value);
}

object*
sub_proc (object *arguments)
{
    long result;
    
    result = (car(arguments))->data.fixnum.value;
    while ((arguments = cdr(arguments))->type != NIL) {
        result -= (car(arguments))->data.fixnum.value;
    }
    return make_fixnum(result);
}

object*
mul_proc (object *arguments)
{
    long result = 1;
    
    while (!(arguments->type == NIL)) {
        result *= (car(arguments))->data.fixnum.value;
        arguments = cdr(arguments);
    }
    return make_fixnum(result);
}

object*
quotient_proc (object *arguments)
{
    return make_fixnum(
        ((car(arguments) )->data.fixnum.value)/
        ((cadr(arguments))->data.fixnum.value));
}

object*
remainder_proc (object *arguments)
{
    return make_fixnum(
        ((car(arguments) )->data.fixnum.value)%
        ((cadr(arguments))->data.fixnum.value));
}

object*
is_number_equal_proc (object *arguments)
{
    long value;
    
    value = (car(arguments))->data.fixnum.value;
    while (!(arguments = cdr(arguments))->type == NIL) {
        if (value != ((car(arguments))->data.fixnum.value)) {
            return f;
        }
    }
    return t;
}

object*
is_less_than_proc (object *arguments)
{
    long previous;
    long next;
    
    previous = (car(arguments))->data.fixnum.value;
    while ((arguments = cdr(arguments))->type != NIL) {
        next = (car(arguments))->data.fixnum.value;
        if (previous < next) {
            previous = next;
        }
        else {
            return f;
        }
    }
    return t;
}

object*
is_greater_than_proc (object *arguments)
{
    long previous;
    long next;
    
    previous = (car(arguments))->data.fixnum.value;
    while ((arguments = cdr(arguments))->type != NIL) {
        next = (car(arguments))->data.fixnum.value;
        if (previous > next) {
            previous = next;
        }
        else {
            return f;
        }
    }
    return t;
}

/*
 * Extracts the arguments of a variable arity function call
 * returns them in a list
 */
object*
list_of_values(object *exps, object *env)
{
    if (exps->type == NIL) {
        return nil;
    }
    else 
    {
        return cons(eval(car (exps), env),
                    list_of_values(cdr (exps), env));
    }
}


object*
cons_proc (object *arguments)
{
    return cons(car(arguments), cadr(arguments));
}

object*
car_proc (object *arguments)
{
    return caar(arguments);
}

object*
cdr_proc (object *arguments)
{
    return cdar(arguments);
}

object*
set_car_proc (object *arguments)
{
    set_car(car(arguments), cadr(arguments));
    return ok;
}

object*
set_cdr_proc(object *arguments)
{
    set_cdr(car(arguments), cadr(arguments));
    return ok;
}

object*
list_proc(object *arguments)
{
    return arguments;
}

object*
is_eq_proc (object *arguments)
{
    object *obj1;
    object *obj2;
    
    obj1 = car(arguments);
    obj2 = cadr(arguments);
    
    if (obj1->type != obj2->type) {
        return f;
    }
    switch (obj1->type) {
        case FIXNUM:
            return (obj1->data.fixnum.value == 
                    obj2->data.fixnum.value) ?
                        t : f;
            break;
        case CHARACTER:
            return (obj1->data.character.value == 
                    obj2->data.character.value) ?
                        t : f;
            break;
        case STRING:
            return (strcmp(obj1->data.string.value, 
                           obj2->data.string.value) == 0) ?
                        t : f;
            break;
        default:
            return (obj1 == obj2) ? t: f;
    }
}

object*
add_proc (object *arg)
{
    long result = 0;
    while (arg->type != NIL)
    {
        result += (car (arg))->data.fixnum.value;
        arg = cdr (arg);
    }
    return make_fixnum (result);
}

/*
 * These three functions are also bound tosymbols, but are different from the
 * above in that they allow creation of environments for the purposes of eval
 */

object*
curr_env_proc (object *arg)
{
    return global_env;
}

object*
new_env_proc (object *arg)
{
    return setup_env();
}

object*
toplevel_env_proc (object *arg)
{
    return make_env ();
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

void 
set_car (object *obj, object* value) {
    obj->data.pair.car = value;
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

void 
set_cdr (object *obj, object* value) {
    obj->data.pair.cdr = value;
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

bool
is_self_evaluating (object *o)
{
    object_t ty = o->type;
    return (ty == BOOLEAN || ty == FIXNUM || ty == CHARACTER || ty == STRING);
}

object *
get_apply_arguments (object *arguments)
{
    if ((cdr (arguments))->type == NIL)
        return car (arguments);

    return cons (car (arguments), get_apply_arguments (cdr (arguments)));
}

/* Evaluator of scheme expressions. Self evaluating atoms are returned as is,
 * while tokens representing operators are applied to arguments. This function
 * simulates tail call optimization by using the same stack for tail recursive
 * calls via a goto and variable renaming
 */
object*
eval (object *exp, object *env)
{

tailcall:
    if (is_self_evaluating (exp))
        return exp;
    /* quotes symbol back to screen */
    else if (has_symbol (quote, exp))
        return cadr (exp);
    /* Sets previously defined variable */
    else if (has_symbol (set, exp))
    {
        set_variable (cadr (exp), eval (caddr (exp), env),env);
        return ok;
    }
    /* creates/redefines varisable in current scope */
    else if (has_symbol (define, exp))
    {
        object *def_val, *def_var;
        /* If the defined thing is bound to a symbol, we define the value as the
         * supplied value
         */
        if ((cadr (exp))->type == SYMBOL)
            def_val = caddr (exp);
        /* Otherwise we are usoing the shortform for a lambda, so we create a
         * lambda expression to eval
         */
        else
           def_val = cons(lambda, cons(cdadr (exp), cddr (exp)));
        if ((cadr (exp))->type == SYMBOL)
            def_var = cadr (exp);
        else
            def_var = caadr (exp);

        define_variable (def_var, eval (def_val, env),env);
        return ok;
    }
    /* Mandated by R5RS, h=implementation of tail calls */
    else if (has_symbol (begin, exp))
    {
        exp = begin_actions (exp);
        while ((cdr (exp))->type != NIL)
        {
            eval (car (exp), env);
            exp = cdr (exp);
        }
        exp = car (exp);
        goto tailcall;
    }

    else if (has_symbol (ifs, exp))
    {
        object *if_predicate = cadr(exp);
        object *if_consequent = caddr(exp);
        object *if_alternative = (cdddr (exp) == nil)? f : cadddr(exp);
        object *result = eval (if_predicate, env);
        if (result != f)
            exp = if_consequent;
        else
            exp = if_alternative;
        goto tailcall;
    }
    /* short circuited and and or */
    else if (has_symbol (and, exp))
    {
        object *result;
        exp = cdr (exp);
        if (exp->type == NIL)
            return t;
        while ((cdr (exp))->type != NIL)
        {
            result = eval (car (exp), env);
            if (result == f)
                return f;
            exp = cdr (exp);
        }
        exp = car (exp);
        goto tailcall;
    }
    else if (has_symbol (or, exp))
    {
        object *result;
        exp = cdr (exp);
        if (exp->type == NIL)
            return f;
        while ((cdr (exp))->type != NIL)
        {
            result = eval (car (exp), env);
            if (result == t)
                return t;
            exp = cdr (exp);
        }
        exp = car (exp);
        goto tailcall;
    }
    /* Anonymous function definitions */ 
    else if (has_symbol (lambda, exp))
    {
        object *params = cadr (exp);
        object *body = cddr (exp);
        return make_compound_proc (params, body, env);
    }
    /* Symbol evaluator */
    else if (exp->type == SYMBOL)
        return lookup_variable (exp, env);

    /* This is a beast. If we've come here in eval, that means we have a form,
     * which is a lisp/scheme construct surrounded by parenthesis. We have to
     * deconstruct the form into it's constituent partys and recursively
     * evaluate all those parts, which may themselves be forms. Since we've
     * reached this point we can assume we're at some sort of application
     */
    else if (exp->type == PAIR)
    {
        /* First we get the procedure and is arguments */
        object *procedure = eval (car (exp), env);
        object *arguments = list_of_values (cdr (exp), env);
        /* If the procedure is apply, we treat it slightly differently. Then the
         * real procedure is the first argument of apply, and the arguments are
         * the rest of the members of the form
         */
        if (procedure->type == PRIM_PROC 
                && procedure->data.prim_proc.fun == apply_proc)
        {
            procedure = car (arguments);
            arguments = get_apply_arguments (cdr (arguments));
        }
        /* If the procedure is eval, we treat it differently. The first argument
         * is an expression to evaluate and the second is an environment to
         * evaluate it in. We get those and tail recursively evaluate the exp
         */
        if (procedure->type == PRIM_PROC 
                && procedure->data.prim_proc.fun == eval_proc)
        {
            exp = car (arguments);
            env = cadr (arguments);
            goto tailcall;
        }
        /* Otherwise, if it's a primitive (library) procedure, we apply it to
         * its arguments
         */
        if (procedure->type == PRIM_PROC)
            return (procedure->data.prim_proc.fun)(arguments);
        /* If we are applying a compound (user defined) procedure, we add a
         * env frame (like a stack frame in C) with the procedure variables and
         * their supplied values and evaluate the body in that new frame
         */
        else if (procedure->type == COMPOUND_PROC)
        {
            env = extend_env( 
                       procedure->data.compound_proc.parameters,
                       arguments,
                       procedure->data.compound_proc.env);
            exp = cons (begin, procedure->data.compound_proc.body);
            goto tailcall;
        }
   }
   else
   {
        fprintf (stderr, "expression has unknown type");
        exit (1);
   }
   return ok;
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
        case PRIM_PROC:
            printf ("#<procedure>");
            break;
        case COMPOUND_PROC:
            printf ("#<procedure>");
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

/* The following two functions are dummpy library procedures that exist only so
 * we can bind them to symbols to make them callable. These are never called,
 * but are checked for in eval
 */
object*
apply_proc (object *ignore)
{
    return NULL;
}
object*
eval_proc (object *ignore)
{
    return NULL;
}

object*
make_env (void)
{
    object *env = setup_env();
    populate_env (env);
    return env;
}

void
add_procedure (char *name, object *(*fun)(struct object *arguments), object *env)
{
    define_variable (make_symbol (name), make_primitive_proc(fun), env);
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

    global_env = make_env ();


    quote = make_symbol ("quote");
    define = make_symbol ("define");
    set = make_symbol ("set!");
    ok = make_symbol ("ok");
    ifs = make_symbol ("if");
    lambda = make_symbol ("lambda");
    begin = make_symbol ("begin");
    cond = make_symbol ("cond");
    and = make_symbol ("and");
    or = make_symbol ("or");
}

/* Adds library procedures to a given argument */
void
populate_env (object *env)
{
    add_procedure("null?"     , is_null_proc, env);
    add_procedure("boolean?"  , is_boolean_proc, env);
    add_procedure("symbol?"   , is_symbol_proc, env);
    add_procedure("integer?"  , is_integer_proc, env);
    add_procedure("char?"     , is_char_proc, env);
    add_procedure("string?"   , is_string_proc, env);
    add_procedure("pair?"     , is_pair_proc, env);
    add_procedure("procedure?", is_procedure_proc, env);
    
    add_procedure("char->integer" , char_to_integer_proc, env);
    add_procedure("integer->char" , integer_to_char_proc, env);
    add_procedure("number->string", number_to_string_proc, env);
    add_procedure("string->number", string_to_number_proc, env);
    add_procedure("symbol->string", symbol_to_string_proc, env);
    add_procedure("string->symbol", string_to_symbol_proc, env);
      
    add_procedure("+"        , add_proc, env);
    add_procedure("-"        , sub_proc, env);
    add_procedure("*"        , mul_proc, env);
    add_procedure("quotient" , quotient_proc, env);
    add_procedure("remainder", remainder_proc, env);
    add_procedure("="        , is_number_equal_proc, env);
    add_procedure("<"        , is_less_than_proc, env);
    add_procedure(">"        , is_greater_than_proc, env);

    add_procedure("cons"    , cons_proc, env);
    add_procedure("car"     , car_proc, env);
    add_procedure("cdr"     , cdr_proc, env);
    add_procedure("set-car!", set_car_proc, env);
    add_procedure("set-cdr!", set_cdr_proc, env);
    add_procedure("list"    , list_proc, env);

    add_procedure("eq?", is_eq_proc, env);
    add_procedure("apply", apply_proc, env);
    add_procedure("eval", eval_proc, env);

    add_procedure("new", new_env_proc, env);
    add_procedure("currenv", curr_env_proc, env);
    add_procedure("toplevelenv", toplevel_env_proc, env);
    add_procedure("eval", eval_proc, env);
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

/* Creates a symbol IR and adds it to the symbol table */
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
    if (!silent)
        printf ("Welcome to Scum, the shitty Scheme interpreter!\n");
    while (1)
    {
        if (!silent)
        {
            printf ("%d> ", instr_count++);
            write (eval (read (in), global_env));
            printf ("\n");
        }
        else
        {
            eval (read (in), global_env);
        }
    }
    fclose (in);
}

/* Creates an empty environment */
object*
setup_env (void)
{
    return extend_env(nil, nil, nil);
}

/* Adds a frame to the top of the 'stack' containing the specified variable and
 * value lists. The values in the value list correspond to the symbols in thje
 * vars list in order 
 */
object 
*extend_env(object *vars, object *vals, object *base_env) 
{
    return cons(make_frame(vars, vals), base_env);
}

void
add_binding (object *var, object *val, object *frame)
{
    set_car(frame, cons(var, car(frame)));
    set_cdr(frame, cons(val, cdr(frame)));
}

/* looks up a variable in the entirety of the env */
object*
lookup_variable (object *var, object *env)
{
    object *frame, *vars, *vals;
    while (env->type != NIL)
    {
        frame = first_frame (env);
        vars = frame_variables(frame);
        vals = frame_values(frame);
        while (vars->type != NIL)
        {
            if (var == car (vars))
                return car (vals);
            vars = cdr (vars);
            vals = cdr (vals);
        }
        env = enclosing_env(env);
    }
    fprintf (stderr, "Unbound variable, could not lookup\n");
    exit (1);
}

void
set_variable (object *var, object *val, object *env)
{
    object *frame, *vars, *vals;
    while (env->type != NIL)
    {
        frame = first_frame (env);
        vars = frame_variables(frame);
        vals = frame_values(frame);
        while (vars->type != NIL)
        {
            if (var == car (vars))
            {
                set_car (vals, val);
                return;
            }
            vars = cdr (vars);
            vals = cdr (vals);
        }
        env = enclosing_env(env);
    }
    fprintf (stderr, "Unbound variable, could not set\n");
    exit (1);
}

void
define_variable (object *var, object *val, object *env)
{
    object *frame, *vars, *vals;
    frame = first_frame (env);
    vars = frame_variables(frame);
    vals = frame_values(frame);
    while (vars->type != NIL)
    {
        if (var == car (vars))
        {
            set_car (vals, val);
            return;
        }
        vars = cdr (vars);
        vals = cdr (vals);
    }
    add_binding (var, val, frame);
}
