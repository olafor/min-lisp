#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "min-lisp.h"
#include "recursive_descent_tree.h"
#include "tokens.h"

#define DEBUG 0
#define ERROR 0

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

char*
lisp_parse_state_as_string (ParseState ps) {
    switch (ps) {
        case PARSE_STATE_NONE:
            return "PARSE_STATE_NONE";
        case PARSE_STATE_LEFT_PAR:
            return "PARSE_STATE_LEFT_PAR";
        case PARSE_STATE_OP:
            return "PARSE_STATE_OP";
        case PARSE_STATE_ONE_VALUE:
            return "PARSE_STATE_ONE_VALUE";
        case PARSE_STATE_TWO_VALUES:
            return "PARSE_STATE_TWO_VALUES";
        case PARSE_STATE_IF:
            return "PARSE_STATE_IF";
        case PARSE_STATE_WHILE:
            return "PARSE_STATE_WHILE";
        case PARSE_STATE_FOR:
            return "PARSE_STATE_FOR";
        case PARSE_STATE_COMPOSE:
            return "PARSE_STATE_COMPOSE";
        case PARSE_STATE_RIGHT_PAR:
            return "PARSE_STATE_RIGHT_PAR";
    }
    return 0;
}

struct _lisp {
    //LispHaspTable *env;
    bool compose; 
    char *expr;
    int error; 
    size_t expr_size;
    RdtNode *rdt_tree;
    RdtNode *current_rdt_node;
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
    lisp->rdt_tree = 0;
    lisp->current_rdt_node = 0;

    return lisp;
}

void
lisp_free (Lisp * lisp) {
    LISP_DEBUG;
    if (!lisp)
        return;

    rdt_node_free (lisp->rdt_tree);

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

bool
lisp_val_from_string (Lisp * lisp, int * index, int * num) {
    char curr_c = lisp->expr[*index];
    int num_i_start = -1;

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
                *num = atoi (lisp->expr + num_i_start);
                rdt_node_add_val (lisp->current_rdt_node, *num);
                *index -= 2;
                return TRUE;
            }
            return FALSE;
        }
    }
}

int
lisp_parse (Lisp * lisp, int * index) {
    ParseState parser_state = PARSE_STATE_NONE;
    RdtNode *rtd_node_parent;
    Token current_token = NO_TOKEN;
    Token op = NO_TOKEN;
    int left_val = -1;
    int right_val = -1;
    int result = -1;

    LISP_DEBUG;

    if (!lisp->rdt_tree) {
        lisp->rdt_tree = rdt_node_init (0, 0);
        lisp->current_rdt_node = lisp->rdt_tree;
    }
    else {
        lisp->current_rdt_node = rdt_node_add_child (lisp->current_rdt_node);
    }

    for (;; ++(*index)) {
        LISP_DEBUG;
        current_token = token_from_string (lisp->expr, lisp->expr_size, index);

        #if DEBUG
            printf ("<token: %s>\n", token_as_string (current_token));
            printf ("<parse state: %s>\n", lisp_parse_state_as_string (parser_state));
        #endif

        if (isspace (lisp->expr[*index])) {
            LISP_DEBUG;
            continue;
        }

        switch (parser_state) {
            case PARSE_STATE_NONE:
                switch (current_token) {
                    case LEFT_PARENTHESIS:
                        parser_state = PARSE_STATE_LEFT_PAR;
                        break;
                    default:
                        LISP_ERROR;
                        goto erroneous_expr;
                }
                break;
            case PARSE_STATE_LEFT_PAR:
                if (token_is_op (current_token) && !token_is_op (op)) {
                    op = current_token;
                    rdt_node_add_op (lisp->current_rdt_node, token_as_string (op));
                    parser_state = PARSE_STATE_OP;
                }
                else {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
                break;
            case PARSE_STATE_OP:
                if (current_token == LEFT_PARENTHESIS) {
                    left_val = lisp_parse (lisp, index);
                    #if (DEBUG)
                        printf ("new left val: %d\n", left_val);
                    #endif
                    parser_state = PARSE_STATE_ONE_VALUE;
                }
                else if (lisp_val_from_string (lisp, index, &left_val)) {
                    #if (DEBUG)
                        printf ("new left val: %d\n", left_val);
                    #endif
                    parser_state = PARSE_STATE_ONE_VALUE;
                }
                else {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
                break;
            case PARSE_STATE_ONE_VALUE:
                if (current_token == LEFT_PARENTHESIS) {
                    right_val = lisp_parse (lisp, index);
                    #if (DEBUG)
                        printf ("new right val: %d\n", right_val);
                    #endif
                    parser_state = PARSE_STATE_TWO_VALUES;
                }
                else if (lisp_val_from_string (lisp, index, &right_val)) {
                    #if (DEBUG)
                        printf ("new right val: %d\n", right_val);
                    #endif
                    parser_state = PARSE_STATE_TWO_VALUES;
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
                        parser_state = PARSE_STATE_RIGHT_PAR;
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
    }
erroneous_expr:
    printf ("not supported (yet?)\n");
    return result;
return_result:
    result = lisp_evaluate (lisp, op, left_val, right_val);
    rdt_node_add_res (lisp->current_rdt_node, result);
    rtd_node_parent = rdt_node_get_parent (lisp->current_rdt_node);
    if (rtd_node_parent)
        lisp->current_rdt_node = rtd_node_parent;
    return result;
}

void
lisp_print_rdt_tree (Lisp * lisp) {
    rdt_node_print_tree (lisp->rdt_tree);
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

    result = lisp_parse (lisp, &index);
    *error = lisp->error;
    return result;
}
