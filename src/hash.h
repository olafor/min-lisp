#ifndef _LISP_HASH_TABLE_H_
#define _LISP_HASH_TABLE_H_

typedef struct _lisp_hash_table LispHashTable;

struct _lisp_hash_table {
    
};

LispHashTable *lisp_hash_table_init (void);

void lisp_hash_table_free (LispHashTable * lh);

void lisp_hash_table_insert (const char * s);

char *lisp_hash_table_get (const char * s);

char *lisp_hash_table_remove (const char * s);





#endif /* _LISP_HASH_TABLE_H_ */
