#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

/* Internal representation of Scheme objects/data */
typedef enum { FIXNUM, BOOLEAN, CHARACTER } object_t;

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
    } data;
} object;

/* Functions used to read input from files */
bool is_delimiter (int);
int peek (FILE*);
void rem_whitespace (FILE*);
bool is_next_input (FILE*, char*);
char read_character (FILE*);
object *read(FILE*);

/* Functions to create IR structures from string file input */
object *alloc_object (void);
object *make_fixnum (long);
object *make_boolean (bool);
object *make_character (char);

/* Functions used to evaluate Scheme code */
object *eval (object*);

/* Functions used to write back to user */
void write (object*);

