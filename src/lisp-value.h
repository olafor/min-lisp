#ifndef _lisp_value_h_
#define _lisp_value_h_

#include <stdlib.h>
#include <string.h>

#define MAX_STR_SIZE 512
#define MAX_KEY_SIZE 128

typedef enum _supported_lisp_value {
    NO_TYPE,
    POS_INT_TYPE,
    KEYVAL_TYPE,
    STR_TYPE
} LispValueType;

typedef struct _lisp_value LispValue;

LispValue *lisp_value_init (LispValueType type);

void lisp_value_free (LispValue ** lv);

LispValue *lisp_value_ref (LispValue * lv);

void lisp_value_unref (LispValue * lv);

void lisp_value_chain_lv (LispValue * first_lv, LispValue * new_lv);

int lisp_value_remove_by_key (LispValue * lv, char * key);

LispValue *lisp_value_get_by_key (LispValue * lv, char * key);

void lisp_value_set_key (LispValue * lv, char *name);

void lisp_value_mark_env_retreival (LispValue * lv);

int lisp_value_is_marked_for_retreival (LispValue * lv);

void lisp_value_set_nonstr (LispValue * lv, void * val, int size);

void lisp_value_set_str (LispValue * lv, void * val, int size);

void *lisp_value_get (LispValue * lv);

char *lisp_value_get_key (LispValue * lv);

LispValueType lisp_value_get_type (const LispValue * lv);

char *lisp_value_get_str (LispValue * lv);

int lisp_value_is_true (const LispValue * lv);

#endif /* _lisp_value_h_ */
