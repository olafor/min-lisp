#include "lisp-value.h"

struct _lisp_value {
    LispValueType type;
    void *val;
    char *str_form;
    int refcount;
};

LispValue *
lisp_value_init (LispValueType type) {
    LispValue *lv;
    
    lv = (LispValue*) calloc (1, sizeof (LispValue));

    lv->type = type;
    lv->val = 0;
    lv->str_form = 0;
    lv->refcount = 1;

    return lv;
}

void
lisp_value_free (LispValue * lv) {
    if (!lv)
        return;

    if (lv->val)
        free (lv->val);
    if (lv->str_form)
        free (lv->str_form);

    free (lv);
    lv = 0;
}

LispValue *
lisp_value_ref (LispValue * lv) {
    lv->refcount += 1;
    return lv;
}

void
lisp_value_unref (LispValue * lv) {
    if (--(lv->refcount) < 1)
        lisp_value_free (lv);
}

void
lisp_value_set_nonstr (LispValue * lv, void * val, int size) {
    lv->val = (int*) calloc (1, size);
    memcpy (lv->val, val, size);
}

void
lisp_value_set_str (LispValue * lv, void * val, int size) {
    lv->str_form = calloc (1 * size + 1, sizeof (char));
    memcpy (lv->str_form, val, size);
}

void *
lisp_value_get (const LispValue * lv) {
    switch (lv->type) {
        case NO_TYPE:
            return 0;
        case STR_TYPE:
            return lv->str_form;
        default:
            return lv->val;
    }
}

LispValueType
lisp_value_get_type (const LispValue * lv) {
    return lv->type;
}

char *
lisp_value_get_str (const LispValue * lv) {
    return lv->str_form;
}

int lisp_value_is_true (const LispValue * lv) {
    int *tmp = 0;
    tmp = (int*) lv->val;
    return *tmp;
}
