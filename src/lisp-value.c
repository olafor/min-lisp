#include <stdio.h>

#include "lisp-value.h"

/* FIXME: bättre minneshantering här */
struct _lisp_value {
    LispValue *next_lv;
    LispValueType type;
    char *key;
    char str_form[MAX_STR_SIZE];
    int is_env_retreival;
    int refcount;
    void *val;
};

LispValue *
lisp_value_init (LispValueType type) {
    LispValue *lv;
    
    lv = (LispValue*) calloc (1, sizeof (LispValue));

    lv->is_env_retreival = 0;
    lv->key = NULL;
    lv->next_lv = NULL;
    lv->refcount = 1;
    lv->type = type;
    lv->val = NULL;

    return lv;
}

void
lisp_value_free (LispValue ** l) {
    if (!l)
        return;

    LispValue *lv = *l;

    if (!lv)
        return;

    if (lv->val) {
        free (lv->val);
        lv->val = NULL;
    }
    if (lv->key) {
        free (lv->key);
        lv->key = NULL;
    }
    if (lv->next_lv)
        lisp_value_free (&(lv->next_lv));

    free (lv);
    lv = NULL;
}

void
lisp_value_chain_lv (LispValue * first_lv, LispValue * new_lv) {
    LispValue *prev_lv = NULL;
    LispValue *tmp_lv = NULL;

    if (!first_lv || !new_lv)
        return;

    tmp_lv = first_lv;

    while ((strcmp (tmp_lv->key, new_lv->key) != 0) || tmp_lv->next_lv) {
        if (strcmp (tmp_lv->key, new_lv->key) == 0) {
            if (prev_lv)
                prev_lv->next_lv = new_lv;
            new_lv->next_lv = tmp_lv->next_lv;
            lisp_value_free (&tmp_lv);
            return;
        }
        prev_lv = tmp_lv;
        tmp_lv = tmp_lv->next_lv;
    }
    tmp_lv->next_lv = new_lv;
}

int
lisp_value_remove_by_key (LispValue * lv, char * key) {
    LispValue *prev_lv = NULL;
    LispValue *tmp_lv = lv;

    while ((strcmp (tmp_lv->key, key) != 0) && (tmp_lv->next_lv)) {
        prev_lv = tmp_lv;
        tmp_lv = tmp_lv->next_lv;
    }

    if (strcmp (tmp_lv->key, key) == 0) {
        if (prev_lv && tmp_lv->next_lv)
            prev_lv->next_lv = tmp_lv->next_lv;
        tmp_lv->next_lv = NULL;
        lisp_value_unref (tmp_lv);
        if (!tmp_lv)
            return 1;
    }
    return 0;
}

void
lisp_value_mark_env_retreival (LispValue * lv) {
    lv->is_env_retreival = 1;
}

int
lisp_value_is_marked_for_retreival (LispValue * lv) {
    return lv->is_env_retreival;
}

LispValue*
lisp_value_get_by_key (LispValue * lv, char * key) {
    LispValue *tmp_lv = lv;

    do {
        if (tmp_lv->key != NULL) {
            if (strcmp (tmp_lv->key, key) == 0)
                return tmp_lv;
        }
        if (!tmp_lv->next_lv) {
            return NULL;
        }
        tmp_lv = tmp_lv->next_lv;
    } while (tmp_lv != NULL);
}

LispValue *
lisp_value_ref (LispValue * lv) {
    lv->refcount += 1;
    return lv;
}

void
lisp_value_unref (LispValue * lv) {
    if (!lv)
        return;
    if (--(lv->refcount) < 1)
        lisp_value_free (&lv);
}

void
lisp_value_set_key (LispValue * lv, char *key) {
    if (key)
        lv->key = strdup (key);
}

void
lisp_value_set_nonstr (LispValue * lv, void * val, int size) {
    lv->val = calloc (1, size);
    memcpy (lv->val, val, size);
}

void
lisp_value_set_str (LispValue * lv, void * val, int size) {
    if (size < MAX_STR_SIZE)
        memcpy (lv->str_form, val, size);
}

void *
lisp_value_get (LispValue * lv) {
    if (!lv)
        return NULL;

    switch (lv->type) {
        case NO_TYPE:
            return NULL;
        case STR_TYPE:
            return lv->str_form;
        default:
            return lv->val;
    }
}

char *
lisp_value_get_key (LispValue * lv) {
    return lv->key;
}

LispValueType
lisp_value_get_type (const LispValue * lv) {
    return lv->type;
}

char *
lisp_value_get_str (LispValue * lv) {
    return lv->str_form;
}

int lisp_value_is_true (const LispValue * lv) {
    int *tmp = NULL;
    tmp = (int*) lv->val;
    return *tmp;
}
