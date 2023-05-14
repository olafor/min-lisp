#include "hash.h"

LispHashTable *lisp_hash_table_init (void);
void lisp_hash_table_free (LispHashTable * lh);
void lisp_hash_table_insert (const char * s);
char *lisp_hash_table_get (const char * s);
char *lisp_hash_table_remove (const char * s);
