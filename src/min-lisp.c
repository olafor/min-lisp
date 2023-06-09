#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lisp-value.h"
#include "min-lisp.h"
#include "recursive-descent-tree.h"
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
    PARSE_STATE_IF,
    PARSE_STATE_OP,
    PARSE_STATE_ONE_VALUE,
    PARSE_STATE_TWO_VALUES,
    PARSE_STATE_THREE_VALUES,
    PARSE_STATE_RIGHT_PAR
} ParseState;

typedef enum _statement {
    NO_STATEMENT,
    QUOTE_STATEMENT,
    IF_STATEMENT,
    FOR_STATEMENT,
    WHILE_STATEMENT,
    COMPOSE_STATEMENT
} Statement;

char*
lisp_parse_state_as_string (ParseState ps) {
    switch (ps) {
        case PARSE_STATE_NONE:
            return "PARSE_STATE_NONE";
        case PARSE_STATE_LEFT_PAR:
            return "PARSE_STATE_LEFT_PAR";
        case PARSE_STATE_IF:
            return "PARSE_STATE_IF";
        case PARSE_STATE_OP:
            return "PARSE_STATE_OP";
        case PARSE_STATE_ONE_VALUE:
            return "PARSE_STATE_ONE_VALUE";
        case PARSE_STATE_TWO_VALUES:
            return "PARSE_STATE_TWO_VALUES";
        case PARSE_STATE_THREE_VALUES:
            return "PARSE_STATE_THREE_VALUES";
        case PARSE_STATE_RIGHT_PAR:
            return "PARSE_STATE_RIGHT_PAR";
    }
    return 0;
}

char *
lisp_parse_statement_as_string (Statement st) {
    switch (st) {
        case NO_STATEMENT:
            return "NO_STATEMENT";
        case IF_STATEMENT:
            return "IF";
        case FOR_STATEMENT:
            return "FOR";
        case WHILE_STATEMENT:
            return "WHILE";
        case QUOTE_STATEMENT:
            return "QUOTE";
        case COMPOSE_STATEMENT:
            return "COMPOSE";
    }
}

struct _lisp {
    //LispHashTable *env;
    char *expr;
    int error; 
    size_t expr_size;;
    RdtNode *rdt_tree;
    RdtNode *current_rdt_node;
};

Statement
lisp_parse_check_if_statement (Lisp * lisp, Statement existing, Token token) {
    LISP_DEBUG;
    if (existing)
        return existing;

    Statement statement;

    switch (token) {
        case IF:
            statement = IF_STATEMENT;
            break;
        default:
            return NO_STATEMENT;
    }

    rdt_node_add_statement (lisp->current_rdt_node,
            lisp_parse_statement_as_string (statement));

    return statement;
}

Lisp *
lisp_init (void) {
    Lisp *lisp;
    LISP_DEBUG;

    lisp = (Lisp*) calloc (1, sizeof (Lisp));

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

LispValue*
lisp_evaluate (Lisp * lisp, const Token op, const LispValue * val_a,
                                            const LispValue * val_b) {
    LISP_DEBUG;
    int *a = (int*) lisp_value_get (val_a);
    int *b = (int*) lisp_value_get (val_b);
    int tmp = 0;
    LispValue *result = lisp_value_init (POS_INT_TYPE);

    switch (op) {
        case EQ:
            tmp = *a == *b;
            lisp_value_set_nonstr (result, &tmp, sizeof (int));
            return result;
        case GT:
            tmp = *a > *b;
            lisp_value_set_nonstr (result, &tmp, sizeof (int));
            return result;
        case GTE:
            tmp = *a >= *b;
            lisp_value_set_nonstr (result, &tmp, sizeof (int));
            return result;
        case LT:
            tmp = *a < *b;
            lisp_value_set_nonstr (result, &tmp, sizeof (int));
            return result;
        case LTE:
            tmp = *a <= *b;
            lisp_value_set_nonstr (result, &tmp, sizeof (int));
            return result;
        case ADD:
            tmp = *a + *b;
            lisp_value_set_nonstr (result, &tmp, sizeof (int));
            return result;
        case SUB:
            tmp = *a - *b;
            lisp_value_set_nonstr (result, &tmp, sizeof (int));
            return result;
        case MUL:
            tmp = *a * *b;
            lisp_value_set_nonstr (result, &tmp, sizeof (int));
            return result;
        case MOD:
            tmp = *a % *b;
            lisp_value_set_nonstr (result, &tmp, sizeof (int));
            return result;
        default: {
            lisp->error = 5;
            return 0;
        }
    }
}

bool
lisp_val_from_string (Lisp * lisp, int * index, LispValue * lv) {
    char curr_c = lisp->expr[*index];
    int num_i_start = -1;
    int num;

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
                num = atoi (lisp->expr + num_i_start);
                lisp_value_set_nonstr (lv, (void*)&num, sizeof (int));
                rdt_node_add_val (lisp->current_rdt_node, lv);
                *index -= 2;
                return TRUE;
            }
            return FALSE;
        }
    }
}

LispValue *
lisp_parse (Lisp * lisp, int * index) {
    ParseState parser_state = PARSE_STATE_NONE;
    RdtNode *rtd_node_parent;
    Statement statement = NO_STATEMENT;
    Token current_token = NO_TOKEN;
    Token op = NO_TOKEN;
    LispValue *val_a = 0;
    LispValue *val_b = 0;
    LispValue *val_c = 0;
    LispValue *result = 0;
    int *tmp = 0;

    LISP_DEBUG;

    if (!lisp->rdt_tree) {
        LISP_DEBUG;
        lisp->rdt_tree = rdt_node_init (0, 0);
        lisp->current_rdt_node = lisp->rdt_tree;
    }
    else {
        LISP_DEBUG;
        lisp->current_rdt_node = rdt_node_add_child (lisp->current_rdt_node);
    }

    for (;; ++(*index)) {
        LISP_DEBUG;
        current_token = token_from_string (lisp->expr, lisp->expr_size, index);
        statement = lisp_parse_check_if_statement (lisp, statement, current_token);

#if DEBUG
            printf ("<token      : %s>\n", token_as_string (current_token));
            printf ("<parse state: %s>\n", lisp_parse_state_as_string (parser_state));
            printf ("<statement  : %s>\n", lisp_parse_statement_as_string (statement));
#endif

        if (isspace (lisp->expr[*index])) {
            LISP_DEBUG;
            continue;
        }

        switch (parser_state) {
            case PARSE_STATE_NONE:
                LISP_DEBUG;
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
                LISP_DEBUG;
                if (token_is_op (current_token) && !token_is_op (op)) {
                    LISP_DEBUG;
                    op = current_token;
                    rdt_node_add_op (lisp->current_rdt_node, token_as_string (op));
                    parser_state = PARSE_STATE_OP;
                    break;
                }
                else if (statement == IF_STATEMENT) {
                    LISP_DEBUG;
                    parser_state = PARSE_STATE_IF;
                }
                else {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
            case PARSE_STATE_IF:
            case PARSE_STATE_OP:
                LISP_DEBUG;
                val_a = lisp_value_init (POS_INT_TYPE);
                if (current_token == LEFT_PARENTHESIS) {
                    LISP_DEBUG;
                    val_a = lisp_parse (lisp, index);
                    parser_state = PARSE_STATE_ONE_VALUE;
                }
                else if (lisp_val_from_string (lisp, index, val_a)) {
                    LISP_DEBUG;
                    parser_state = PARSE_STATE_ONE_VALUE;
                }
                else {
                    LISP_ERROR;
                    lisp_value_free (val_a);
                    goto erroneous_expr;
                }
#if DEBUG
                tmp = (int*) lisp_value_get (val_a);
                printf ("val_a: %d\n", *tmp);
#endif
                break;
            case PARSE_STATE_ONE_VALUE:
                LISP_DEBUG;
                val_b = lisp_value_init (POS_INT_TYPE);
                if (current_token == LEFT_PARENTHESIS) {
                    val_b = lisp_parse (lisp, index);
                    parser_state = PARSE_STATE_TWO_VALUES;
                }
                else if (lisp_val_from_string (lisp, index, val_b)) {
                    parser_state = PARSE_STATE_TWO_VALUES;
                }
                else {
                    LISP_ERROR;
                    lisp_value_free (val_b);
                    goto erroneous_expr;
                }
#if DEBUG
                tmp = (int*) lisp_value_get (val_b);
                printf ("val_b: %d\n", *tmp);
#endif
                break;
            case PARSE_STATE_TWO_VALUES:
                LISP_DEBUG;
                if (current_token == RIGHT_PARENTHESIS) {
                    parser_state = PARSE_STATE_RIGHT_PAR;
                    goto result_no_statement;
                }
                if (statement == IF_STATEMENT) {
                    val_c = lisp_value_init (POS_INT_TYPE);
                    if (current_token == LEFT_PARENTHESIS) {
                        val_c = lisp_parse (lisp, index);
                        parser_state = PARSE_STATE_THREE_VALUES;
                    }
                    else if (lisp_val_from_string (lisp, index, val_c)) {
                        parser_state = PARSE_STATE_THREE_VALUES;
                    }
                }
                else {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
#if DEBUG
                tmp = (int*) lisp_value_get (val_c);
                printf ("val_c: %d\n", *tmp);
#endif
                break;
            case PARSE_STATE_THREE_VALUES:
                LISP_DEBUG;
                if (statement == IF_STATEMENT) {
                        LISP_DEBUG;
                        if (current_token == RIGHT_PARENTHESIS) {
                            parser_state = PARSE_STATE_RIGHT_PAR;
                            goto result_if_statement;
                        }
                }
                LISP_ERROR;
                goto erroneous_expr;

        current_token = NO_TOKEN;
        }
    }
erroneous_expr:
    printf ("not supported (yet?)\n");
    return result;
result_if_statement:
    if (lisp_value_is_true (val_a)) {
        result = lisp_value_ref (val_b);
    }
    else {
        result = lisp_value_ref (val_c);
    }
    rdt_node_add_res (lisp->current_rdt_node, result);
    rtd_node_parent = rdt_node_get_parent (lisp->current_rdt_node);
    if (rtd_node_parent)
        lisp->current_rdt_node = rtd_node_parent;

    lisp_value_unref (val_a);
    lisp_value_unref (val_b);
    lisp_value_unref (val_c);

    return result;
result_no_statement:
    result = lisp_evaluate (lisp, op, val_a, val_b);
    rdt_node_add_res (lisp->current_rdt_node, result);
    rtd_node_parent = rdt_node_get_parent (lisp->current_rdt_node);
    if (rtd_node_parent)
        lisp->current_rdt_node = rtd_node_parent;

    lisp_value_free (val_a);
    lisp_value_free (val_b);
    lisp_value_free (val_c);

    return result;
}

void
lisp_print_rdt_tree (Lisp * lisp) {
    rdt_node_print_tree (lisp->rdt_tree);
}

int
lisp_run_expr (Lisp * lisp, char * expr, int * error) {
    LISP_DEBUG;
    int *result = 0;
    int index = 0;

    if (!lisp || !expr)
        return -1;

    if (lisp->expr)
        free (lisp->expr);

    lisp->expr = expr;
    lisp->expr_size = strlen (expr);

    result = (int*) lisp_value_get (lisp_parse (lisp, &index));
    *error = lisp->error;
    return *result;
}
