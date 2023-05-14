#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
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
        default:
            lisp->error = TRUE;
            return 0;
    }
}

int
lisp_parse (Lisp * lisp, const char * raw_expr, int * chars_moved) {
    bool go_deeper;
    bool has_expression_prepared; 
    bool has_val_a; 
    bool value_has_surfaced; 
    char *remaining_expr;
    char op;
    int expr_len;
    int i;
    int result;
    int val_a;
    int val_b;

    expr_len = strlen (raw_expr);
    go_deeper = FALSE;
    has_expression_prepared = FALSE;
    has_val_a = FALSE;
    i = 0;
    op = 0;
    remaining_expr = 0;
    value_has_surfaced = FALSE;

    for (; i < expr_len; ++i) {
        if (lisp->error)
            break;

        if (remaining_expr != 0)
            free (remaining_expr);

        remaining_expr = (char *) calloc(1, expr_len - i + 1);
        memcpy(remaining_expr, raw_expr + i, expr_len - i);

        if (go_deeper) {
            result = lisp_parse (lisp, remaining_expr, &i);
            free (remaining_expr);
            remaining_expr = (char *) calloc(1, expr_len - i + 1);
            memcpy(remaining_expr, raw_expr + i, expr_len - i);
            go_deeper = FALSE;
            value_has_surfaced = TRUE;
        }

        switch (remaining_expr[0]) {
            case ' ':
                continue;
            case '(':
                if (op != 0)
                    go_deeper = TRUE;
                continue;
            case 'c':
                lisp->compose = TRUE;
                continue;
            case ',':
                if (!lisp->compose) {
                    lisp->error = TRUE;
                    break;
                }
            default:
                if (remaining_expr[0] == ')' && !value_has_surfaced) {
                    goto return_res;
                }

                if (_is_op (remaining_expr[0]))
                    op = remaining_expr[0];
                else if (!has_val_a) {
                    if (value_has_surfaced) {
                        val_a = result;
                        value_has_surfaced = FALSE;
                        --i;
                    }
                    else
                        val_a = remaining_expr[0] - '0';

                    has_val_a = TRUE;
                }
                else {
                    if (value_has_surfaced) {
                        val_b = result;
                        value_has_surfaced = FALSE;
                    }
                    else 
                        val_b = remaining_expr[0] - '0';

                    has_val_a = FALSE;
                    has_expression_prepared = TRUE;
                }

                if (has_expression_prepared) {
                    result = lisp_evaluate (lisp, op, val_a, val_b);
                }
        }
    }
return_res:
    *chars_moved += i + 1;
    free (remaining_expr);
    return result;
}

int
lisp_run_expr (Lisp * lisp, const char * expr, int * error) {
    int result, ignore;
    result = lisp_parse (lisp, expr, &ignore);
    *error = lisp->error;
    return result;
}
