#ifndef _SCUM_C
#define _SCUM_C

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_STRING_LEN 1000
#define SYMBOL_TABLE_LEN 100
#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))

#define add_procedure(scheme_name, c_name)              \
    define_variable(make_symbol(scheme_name),           \
                    make_primitive_proc(c_name),        \
                    global_env);
#define enclosing_env(env) cdr(env)
#define first_frame(env) car(env)
#define make_frame(vars, vals) cons(vars, vals)
#define frame_variables(frame) car(frame)
#define frame_values(frame) cdr(frame)

#define begin_actions(exp) cdr(exp)

/* Internal representation of Scheme objects/data */
typedef enum { SYMBOL, PAIR, FIXNUM, BOOLEAN, CHARACTER, STRING, NIL, PRIM_PROC,
                COMPOUND_PROC} object_t;

typedef struct object
{
    object_t type;
    union 
    {
        struct 
        {
            long value;
        } fixnum;
        struct
        {
            bool value;
        } boolean;
        struct
        {
            char value;
        } character;
        struct
        {
            char *value;
        } string;
        struct
        {
            struct object *car;
            struct object *cdr;
        } pair;
        struct
        {
            char *value;
        } symbol;
        struct
        {
            struct object *(*fun)(struct object *arguments);
        } prim_proc;
        struct
        {
            struct object *parameters;
            struct object *body;
            struct object *env;
        } compound_proc;
    } data;
} object;

/* Functions and data structures used to create variables in scopes
 * (collectively called the environment )
 */
void add_binding (object*, object*, object*);
object* lookup_variable (object *, object *);
void set_variable (object *, object *, object*);
void define_variable (object*, object*, object*);
object *setup_env(void);
object *extend_env (object*, object*, object*);

/* Functions used to read input from files ansd tokenize that input */
bool is_delimiter (int);
int peek (FILE*);
void rem_whitespace (FILE*);
bool is_next_input (FILE*, char*);
char read_character (FILE*);
object *read(FILE*);
void read_string (FILE*, char*);
object *read_pair (FILE*);
bool is_symbol_start (int);

/* Functions to create IR structures/tokens from string file input */
object *alloc_object (void);
object *make_fixnum (long);
object *make_boolean (bool);
object *make_character (char);
object *make_string (char*);
object *make_primitive_proc (object *(*fun)(struct object *arguments));
object *make_compound_proc (object *, object *, object *);

/* Functions used to evaluate Scheme code */
object *eval (object*, object *);
bool has_symbol (object*, object*);
bool is_self_evaluating (object *);

/* Functions used to write back to user */
void write (object*);
void write_pair (object*);

/* Functions for manipulating lists  */
object *cons (object*, object*);
object *car (object*);
object *cdr (object*);
void set_car (object *, object *);
void set_cdr (object *, object *);

/* Functions and data structures for managing the symbol table */
typedef struct symbol_table_entry
{
    object *object;
    struct symbol_table_entry *next;
} symbol_table_entry;
unsigned hash (char *);
symbol_table_entry *lookup (char *);
symbol_table_entry *install (object *);
object *make_symbol (char *);

/* For APPLY and EVAL trickery */
object *apply_proc (object *);
object *get_apply_arguments(object *);
object *eval_proc (object *);

static symbol_table_entry *symbol_table[SYMBOL_TABLE_LEN];

void make_singletons (void);

void interpret (FILE *, bool);

static object *t, *f, *nil, *quote, *define, *set, *ok, *ifs, *plus, *lambda, *global_env, *begin,
              *cond, *and, *or;
#endif
