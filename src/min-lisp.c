#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#include "min-lisp.h"

#define NUMBER_OF_OPERATORS 7

typedef enum _bool {FALSE = 0, TRUE} bool;
static char operators[] = {'=', '<', '>', '+', '-', '*', '%'};

struct _lisp {
    //LispHaspTable *env;
    bool error; 
    bool compose; 
};

bool _is_op (char c) {
    for (int i = 0; i < NUMBER_OF_OPERATORS; ++i) {
        if (operators[i] == c)
            return TRUE;
    }
    return FALSE;
}

Lisp *
lisp_init (void) {
    Lisp *lisp;

    lisp = (Lisp*) calloc (1, sizeof (Lisp));

    lisp->error = FALSE;
    lisp->compose = FALSE;

    return lisp;
}

void
lisp_free (Lisp * lisp) {
    if (lisp == 0)
        return;

    free (lisp);
}

int
lisp_evaluate (Lisp * lisp, const char op, const int a, const int b) {
    switch (op) {
        case '=':
            return a == b;
        case '<':
            return a < b;
        case '>':
            return a > b;
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '%':
            return a % b;
        default: {
            lisp->error = TRUE;
            return 0;
        }
    }
}

int
lisp_parse (Lisp * lisp, const char * raw_expr, int * chars_moved) {
    char *current_expr;
    char *term;
    char *tmp;
    char op = 0;
    char term_end;
    char term_start;
    int result = -1;
    int term_len;
    int val_a;
    int val_b;

    current_expr = strdup (raw_expr);
    printf ("current_expr: %s\n", current_expr);
    if (current_expr == 0)
        goto failed_expr;

    for (;;) {
        /* retrieve the next term from the expr */
        for (;;) {
            term = strsep (&current_expr, " ");
            if (term == 0)
                goto return_res;
            term_len = strlen (term);
            if (term_len == 0)
                continue;
            break;
        }

        /* analyze term*/
        term_start = term[0];
        term_end = term[term_len - 1];
        if (term_start == '(') {
            if (_is_op (term_end)) {
                op = term_end;
                continue;
            }
            goto failed_expr;
        }
        else if (term_start == ')') 
            goto failed_expr;
        else if (term_end == ')') {
            for (int i = 0; i < term_len - 1; ++i) {
                if (!isdigit (term[i]))
                    goto failed_expr;
            }
            tmp = calloc (term_len, sizeof (char));
            memcpy (tmp, term, term_len - 1);
            val_b = atoi (tmp);
            free (tmp);
            result = lisp_evaluate (lisp, op, val_a, val_b);
            goto return_res;
        }
        for (int i = 0; i < term_len; ++i) {
            if (!isdigit (term[i]))
                goto failed_expr;
        }
        tmp = calloc (term_len + 1, sizeof (char));
        memcpy (tmp, term, term_len);
        val_a = atoi (tmp);
        free (tmp);
    }
failed_expr:
    printf ("we do not handle this (yet)\n");
    lisp->error = TRUE;
return_res:
    free (current_expr);
    return result;
}

int
lisp_run_expr (Lisp * lisp, const char * expr, int * error) {
    int result, ignore;
    result = lisp_parse (lisp, expr, &ignore);
    *error = lisp->error;
    return result;
}
