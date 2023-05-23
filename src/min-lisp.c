#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "min-lisp.h"
#include "tokens.h"

#define DEBUG 0
#define ERROR 1

#if DEBUG
    #define LISP_DEBUG (printf ("DEBUG %s: %d\n", __FILE__, __LINE__))
#else
    #define LISP_DEBUG
#endif

#if ERROR
    #define LISP_ERROR (printf ("ERROR %s: %d\n", __FILE__, __LINE__))
#else
    #define LISP_ERROR
#endif

typedef enum _parse_state {
    PARSE_STATE_NONE,
    PARSE_STATE_LEFT_PAR,
    PARSE_STATE_OP,
    PARSE_STATE_ONE_VALUE,
    PARSE_STATE_TWO_VALUES,
    PARSE_STATE_IF,
    PARSE_STATE_WHILE,
    PARSE_STATE_FOR,
    PARSE_STATE_COMPOSE,
    PARSE_STATE_RIGHT_PAR
} ParseState;

struct _lisp {
    //LispHaspTable *env;
    ParseState parser_state;
    bool compose; 
    char *expr;
    int error; 
    size_t expr_size;
};

Lisp *
lisp_init (void) {
    Lisp *lisp;
    LISP_DEBUG;

    lisp = (Lisp*) calloc (1, sizeof (Lisp));

    lisp->compose = FALSE;
    lisp->error = 0;
    lisp->expr = 0;
    lisp->expr_size = 0;
    lisp->parser_state = PARSE_STATE_NONE;

    return lisp;
}

void
lisp_free (Lisp * lisp) {
    LISP_DEBUG;
    if (!lisp)
        return;

    free (lisp);
}

int
lisp_evaluate (Lisp * lisp, const Token op, const int a, const int b) {
    LISP_DEBUG;
    switch (op) {
        case EQ:
            return a == b;
        case GT:
            return a > b;
        case GTE:
            return a >= b;
        case LT:
            return a < b;
        case LTE:
            return a <= b;
        case ADD:
            return a + b;
        case SUB:
            return a - b;
        case MUL:
            return a * b;
        case MOD:
            return a % b;
        default: {
            lisp->error = 5;
            return 0;
        }
    }
}

int
lisp_parse (Lisp * lisp, int * index) {
    Token current_token = NO_TOKEN;
    Token op = NO_TOKEN;
    bool has_new_num = FALSE;
    char *num_string = 0;
    char curr_c;
    int left_val = -1;
    int num = -1;
    int num_i_start = -1;
    int right_val = -1;
    LISP_DEBUG;

    for (;; ++(*index)) {
        LISP_DEBUG;
        current_token = token_from_string (lisp->expr, lisp->expr_size, index);

        curr_c = lisp->expr[*index];

        if (isspace (curr_c)) {
            LISP_DEBUG;
            continue;
        }

        if (current_token == NO_TOKEN) {
            LISP_DEBUG;
            for (; *index <= lisp->expr_size; ++(*index)) {
                if (isdigit (curr_c)) {
                    LISP_DEBUG;
                    if (num_i_start == -1)
                        num_i_start = *index;
                    curr_c = lisp->expr[*index];
                }
                else {
                    if (num_i_start > -1) {
                        LISP_DEBUG;
                        --(*index);
                        num_string = (char*) calloc (*index - num_i_start + 1,
                                sizeof (char));
                        memcpy (num_string, lisp->expr + num_i_start,
                                *index - num_i_start);
                        num = atoi (num_string);
                        has_new_num = TRUE;
                        free (num_string);
                        num_i_start = -1;
                        --(*index);
                    }
                    break;
                }
            }
        }

        switch (lisp->parser_state) {
            case PARSE_STATE_NONE:
                switch (current_token) {
                    case LEFT_PARENTHESIS:
                        lisp->parser_state = PARSE_STATE_LEFT_PAR;
                        break;
                    default:
                        LISP_ERROR;
                        goto erroneous_expr;
                }
                break;
            case PARSE_STATE_LEFT_PAR:
                if (token_is_op (current_token) && !token_is_op (op)) {
                    op = current_token;
                    lisp->parser_state = PARSE_STATE_OP;
                }
                else {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
                break;
            case PARSE_STATE_OP:
                if (has_new_num) {
                    left_val = num;
                    lisp->parser_state = PARSE_STATE_ONE_VALUE;
                }
                else if (current_token == LEFT_PARENTHESIS) {
                    left_val = lisp_parse (lisp, index);
                }
                else {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
                break;
            case PARSE_STATE_ONE_VALUE:
                if (has_new_num) {
                    right_val = num;
                    lisp->parser_state = PARSE_STATE_TWO_VALUES;
                }
                else if (current_token == LEFT_PARENTHESIS) {
                    left_val = lisp_parse (lisp, index);
                }
                else {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
                break;
            case PARSE_STATE_TWO_VALUES:
                switch (current_token) {
                    LISP_DEBUG;
                    case RIGHT_PARENTHESIS:
                        lisp->parser_state = PARSE_STATE_RIGHT_PAR;
                        goto return_result;
                    default:
                        LISP_ERROR;
                        goto erroneous_expr;
                }
            default:
                LISP_ERROR;
                goto erroneous_expr;
        }

        current_token = NO_TOKEN;
        has_new_num = FALSE;
    }
erroneous_expr:
    printf ("not supported (yet?)\n");
    return -1;
return_result:
    return lisp_evaluate (lisp, op, left_val, right_val);
}

int
lisp_run_expr (Lisp * lisp, char * expr, int * error) {
    LISP_DEBUG;
    int result;
    int index = 0;

    if (!lisp || !expr)
        return -1;

    if (lisp->expr)
        free (lisp->expr);

    lisp->expr = expr;
    lisp->expr_size = strlen (expr);
    printf ("expr_size: %d\n", lisp->expr_size);

    result = lisp_parse (lisp, &index);
    *error = lisp->error;
    return result;
}
