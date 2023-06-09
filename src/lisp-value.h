#ifndef _lisp_value_h_
#define _lisp_value_h_

#include <stdlib.h>
#include <string.h>

typedef enum _supported_lisp_value {
    NO_TYPE,
    POS_INT_TYPE,
    STR_TYPE
} LispValueType;

typedef struct _lisp_value LispValue;

LispValue *lisp_value_init (LispValueType type);

void lisp_value_free (LispValue * lv);

LispValue *lisp_value_ref (LispValue * lv);

void lisp_value_unref (LispValue * lv);

void lisp_value_set_nonstr (LispValue * lv, void * val, int size);

void lisp_value_set_str (LispValue * lv, void * val, int size);

void *lisp_value_get (const LispValue * lv);

LispValueType lisp_value_get_type (const LispValue * lv);

char *lisp_value_get_str (const LispValue * lv);

int lisp_value_is_true (const LispValue * lv);

#endif /* _lisp_value_h_ */
