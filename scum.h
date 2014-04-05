#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STRING_LEN 1000
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


/* Internal representation of Scheme objects/data */
typedef enum { PAIR, FIXNUM, BOOLEAN, CHARACTER, STRING, NIL } object_t;

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
    } data;
} object;

typedef object symbol;

/* Functions used to read input from files */
bool is_delimiter (int);
int peek (FILE*);
void rem_whitespace (FILE*);
bool is_next_input (FILE*, char*);
char read_character (FILE*);
object *read(FILE*);
void read_string (FILE*, char*);
object *read_list (FILE*);

/* Functions to create IR structures from string file input */
object *alloc_object (void);
object *make_fixnum (long);
object *make_boolean (bool);
object *make_character (char);
object *make_string (char*);

/* Functions used to evaluate Scheme code */
object *eval (object*);

/* Functions used to write back to user */
void write (object*);
void write_list (object*);

/* Functions for manipulating lists  */
object *cons (object*, object*);
object *car (object*);
object *cdr (object*);

void make_singletons (void);

static object *t, *f, *nil;
