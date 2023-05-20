#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "min-lisp.h"

#define NUMBER_OF_OPERATORS 7

typedef enum _bool {FALSE = 0, TRUE} bool;
static char operators[] = {'=', '<', '>', '+', '-', '*', '%'};

struct _lisp {
    //LispHaspTable *env;
    int error; 
    bool compose; 
    size_t expr_size;
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

    lisp->error = 0;
    lisp->compose = FALSE;
    lisp->expr_size = 0;

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
            lisp->error = 5;
            return 0;
        }
    }
}

int
lisp_parse (Lisp * lisp, char * expr, int * index) {
    char curr_c;
    bool has_left_parenthesis = FALSE;
    bool has_left_val = FALSE;
    bool has_right_val = FALSE;
    char op = 0;
    int left_val = 0;
    int right_val = 0;
    int num_i_start = -1;
    char *num_string = 0;
    int num;

    for (;;++(*index)) {
        curr_c = expr[*index];
        if (curr_c == '(') {
            if (has_left_parenthesis && op > 0) {
                if (!has_left_val) {
                    left_val = lisp_parse (lisp, expr, index);
                    has_left_val = TRUE;
                    continue;
                }
                else if (!has_right_val) {
                    right_val = lisp_parse (lisp, expr, index);
                    has_right_val = TRUE;
                    continue;
                }
                else {
                    lisp->error = 1;
                    goto erroneous_expr;
                }

            }
            else {
                has_left_parenthesis = TRUE;
                continue;
            }
        }
        if (curr_c == ')') {
            if (has_left_parenthesis && op > 0 &&
                has_left_val && has_right_val) {
                goto return_result;
            }
            else {
                lisp->error = 2;
                goto erroneous_expr;
            }
        }
        if (_is_op (curr_c) && op == 0 &&
                has_left_parenthesis && !has_left_val) {
            op = curr_c;
            continue;
        }
        for (; *index <= lisp->expr_size; ++(*index)) {
            if (isdigit (curr_c)) {
                if (num_i_start == -1)
                    num_i_start = *index;
                curr_c = expr[*index];
            }
            else {
                if (num_i_start > -1) {
                    --(*index);
                    num_string = (char*) calloc (*index - num_i_start + 1, sizeof (char));
                    memcpy (num_string, expr + num_i_start, *index - num_i_start);
                    num = atoi (num_string);
                    free (num_string);
                    num_i_start = -1;
                    --(*index);
                    if (!has_left_val) {
                        left_val = num;
                        has_left_val = TRUE;
                    }
                    else if (!has_right_val) {
                        right_val = num;
                        has_right_val = TRUE;
                    }
                    else {
                        lisp->error = 3;
                        goto erroneous_expr;
                    }
                }
                break;
            }
        }
    }
erroneous_expr:
    printf ("not supported (yet?)\n");
return_result:
    if (lisp->expr_size == *index)
        free (expr);
    return lisp_evaluate (lisp, op, left_val, right_val);
}

int
lisp_run_expr (Lisp * lisp, char * expr, int * error) {
    int result;
    int index = 0;

    lisp->expr_size = strlen (expr);

    result = lisp_parse (lisp, expr, &index);
    *error = lisp->error;
    return result;
}
