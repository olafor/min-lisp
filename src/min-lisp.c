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

#define TABLE_SIZE 128

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
    PARSE_STATE_DEFVAR_KEY,
    PARSE_STATE_DEFVAR_VAL,
    PARSE_STATE_IF,
    PARSE_STATE_LEFT_PAR,
    PARSE_STATE_ONE_VALUE,
    PARSE_STATE_OP,
    PARSE_STATE_RIGHT_PAR,
    PARSE_STATE_THREE_VALUES,
    PARSE_STATE_TWO_VALUES,
    PARSE_STATE_SEQUENTIAL
} ParseState;

typedef enum _statement {
    NO_STATEMENT,
    COMPOSE_STATEMENT,
    DEFVAR_STATEMENT,
    FOR_STATEMENT,
    IF_STATEMENT,
    QUOTE_STATEMENT,
    WHILE_STATEMENT
} Statement;

char*
lisp_parse_state_as_string (ParseState ps) {
    switch (ps) {
        case PARSE_STATE_NONE:
            return "PARSE_STATE_NONE";
        case PARSE_STATE_LEFT_PAR:
            return "PARSE_STATE_LEFT_PAR";
        case PARSE_STATE_DEFVAR_KEY:
            return "PARSE_STATE_DEFVAR_KEY";
        case PARSE_STATE_DEFVAR_VAL:
            return "PARSE_STATE_DEFVAR_VAL";
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
        case PARSE_STATE_SEQUENTIAL:
            return "PARSE_STATE_SEQUENTIAL";
    }
    return 0;
}

char *
lisp_parse_statement_as_string (Statement st) {
    switch (st) {
        case NO_STATEMENT:
            return "NO_STATEMENT";
        case DEFVAR_STATEMENT:
            return "DEFVAR";
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
    LispValue *env[TABLE_SIZE];
    RdtNode *current_rdt_node;
    RdtNode *rdt_tree;
    char *expr;
    char *key_waiting_for_val;
    int error; 
    size_t expr_size;;
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
        case DEFVAR:
            statement = DEFVAR_STATEMENT;
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

    lisp->current_rdt_node = NULL;
    lisp->error = 0;
    lisp->expr = NULL;
    lisp->expr_size = 0;
    lisp->key_waiting_for_val = NULL;
    lisp->rdt_tree = NULL;

    for (int i = 0; i < TABLE_SIZE; ++i)
        lisp->env[i] = NULL;

    return lisp;
}

void
lisp_free (Lisp * lisp) {
    LISP_DEBUG;
    if (!lisp)
        return;

    if (lisp->key_waiting_for_val)
        free (lisp->key_waiting_for_val);

    for (int i = 0; i < TABLE_SIZE; ++i) {
        if (lisp->env[i]) {
            LispValue *lv = lisp->env[i];
            if (lv)
                lisp_value_free (&lv);
        }
    }

    rdt_node_free (lisp->rdt_tree);

    free (lisp);
}

LispValue*
lisp_evaluate (Lisp * lisp, const Token op, LispValue * val_a,
                                            LispValue * val_b) {
    LISP_DEBUG;
    LispValue *result = lisp_value_init (POS_INT_TYPE);
    int *a = (int*) lisp_value_get (val_a);
    int *b = (int*) lisp_value_get (val_b);
    int tmp = 0;

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
            //lisp->error = 5;
            return 0;
        }
    }
}

int
lisp_get_env_slot (char *name) {
    /* FIXME: bättre hashningsalgoritm... */
    if (name)
        return strlen (name) % TABLE_SIZE;
    else
        return -1;
}

void
lisp_get_key_from_string (Lisp * lisp, int * index) {
    LISP_DEBUG;
    char curr_c = lisp->expr[*index];
    int key_i_end = -1;
    int key_i_start = -1;

    if (lisp->key_waiting_for_val) {
        free (lisp->key_waiting_for_val);
        lisp->key_waiting_for_val = NULL;
    }

    for (; *index <= lisp->expr_size; ++(*index)) {
        if (isalpha (curr_c)) {
            if (key_i_start == -1)
                key_i_start = *index;
            key_i_end = *index;
        }
        if (!isalnum (curr_c) || (key_i_end - key_i_start >= MAX_KEY_SIZE)) {
            LISP_DEBUG;
            *index -= 2;
            lisp->key_waiting_for_val = strndup (lisp->expr + key_i_start, key_i_end - key_i_start);
            return;
        }
        curr_c = lisp->expr[*index];
    }
    LISP_DEBUG;
    return;
}

LispValue*
lisp_check_for_env_val (Lisp * lisp, int * index) {
    LISP_DEBUG;
    LispValue *first_lv = NULL;
    LispValue *lv = NULL;
    int env_slot_i = 0;
    int prev_index_pos = *index;

    lisp_get_key_from_string (lisp, index);

    if (lisp->key_waiting_for_val != NULL) {
        LISP_DEBUG;
        env_slot_i = lisp_get_env_slot (lisp->key_waiting_for_val);
        first_lv = lisp->env[env_slot_i];
        if (first_lv) {
            LISP_DEBUG;
            lv = lisp_value_get_by_key (first_lv, lisp->key_waiting_for_val);

            if (!lv) {
                LISP_DEBUG;
                return NULL;
            }

            return lisp_value_ref (lv);
        }
        if (lisp->key_waiting_for_val) {
            free (lisp->key_waiting_for_val);
            lisp->key_waiting_for_val = NULL;
        }
    }
    LISP_DEBUG;
    *index = prev_index_pos;
    return NULL;
}

LispValue*
lisp_val_from_string (Lisp * lisp, int * index) {
    LISP_DEBUG;
    LispValue *lv = NULL;
    char curr_c = lisp->expr[*index];
    int num;
    int num_i_start = -1;

    lv = lisp_check_for_env_val (lisp, index);

    if (lv)
        return lv;

    for (; *index <= lisp->expr_size; ++(*index)) {
        if (isdigit (curr_c)) {
            if (num_i_start == -1)
                num_i_start = *index;
            curr_c = lisp->expr[*index];
        }
        else {
            if (num_i_start > -1) {
                lv = lisp_value_init (POS_INT_TYPE);
                num = atoi (lisp->expr + num_i_start);
                lisp_value_set_nonstr (lv, (void*)&num, sizeof (int));
                rdt_node_add_val (lisp->current_rdt_node, lv);
                *index -= 2;
                return lv;
            }
            break;
        }
    }
    return NULL;
}


LispValue *
lisp_parse (Lisp * lisp, int * index) {
    LISP_DEBUG;
    LispValue *result = NULL;
    LispValue *val_a = NULL;
    LispValue *val_b = NULL;
    LispValue *val_c = NULL;
    ParseState parser_state = PARSE_STATE_NONE;
    RdtNode *rtd_node_parent;
    Statement statement = NO_STATEMENT;
    Token current_token = NO_TOKEN;
    Token op = NO_TOKEN;
    char *key = NULL;
    int *tmp = NULL;
    int env_slot_i = 0;

#if DEBUG
    printf ("\n\tNy runda av lisp_parse\n");
#endif

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
            printf ("<index      : %d>\n", *index);
            printf ("<char       : %c>\n", lisp->expr[*index]);
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
                if (current_token == LEFT_PARENTHESIS) {
                    LISP_DEBUG;
                    val_a = lisp_parse (lisp, index);
                    if (val_a) {
                        if (lisp_value_is_marked_for_retreival (val_a))
                            return val_a;
                        else {
                            result = val_a;
                            val_a = 0;
                            parser_state = PARSE_STATE_SEQUENTIAL;
                        }
                        break;
                    }
                }
                else if (current_token == RIGHT_PARENTHESIS) {
                    if (val_c)
                        return val_c;
                    else if (val_b)
                        return val_b;
                    else
                        return val_a;
                }
                else if (token_is_op (current_token) && !token_is_op (op)) {
                    LISP_DEBUG;
                    op = current_token;
                    rdt_node_add_op (lisp->current_rdt_node, token_as_string (op));
                    parser_state = PARSE_STATE_OP;
                    break;
                }
                else if (statement == IF_STATEMENT) {
                    LISP_DEBUG;
                    --(*index);
                    parser_state = PARSE_STATE_IF;
                    break;
                }
                else if (statement == DEFVAR_STATEMENT) {
                    LISP_DEBUG;
                    --(*index);
                    parser_state = PARSE_STATE_DEFVAR_KEY;
                    break;
                }
                else {
                    LISP_DEBUG;
                    val_a = lisp_check_for_env_val (lisp, index);
                    if (val_a) {
                        LISP_DEBUG;
                        lisp_value_mark_env_retreival (val_a);
                    }
                }
                break;
            case PARSE_STATE_DEFVAR_KEY:
                LISP_DEBUG;
                lisp_get_key_from_string (lisp, index);
                if (lisp->key_waiting_for_val)
                    parser_state = PARSE_STATE_DEFVAR_VAL;
                else {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
                break;
            case PARSE_STATE_DEFVAR_VAL:
                LISP_DEBUG;
                if (!lisp->key_waiting_for_val) {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
                key = strdup (lisp->key_waiting_for_val);

                if (current_token == LEFT_PARENTHESIS) {
                    val_a = lisp_parse (lisp, index);
                }
                else {
                    val_a = lisp_val_from_string (lisp, index);
                }
                lisp_value_set_key (val_a, key);
#if DEBUG
                if (val_a) {
                    tmp = (int*) lisp_value_get (val_a);
                    printf ("val_a: %d\n", *tmp);
                }
#endif
                env_slot_i = lisp_get_env_slot (key);
                if (lisp->env[env_slot_i])
                    lisp_value_chain_lv (lisp->env[env_slot_i], val_a);
                else
                    lisp->env[env_slot_i] = val_a;

                /* nästa värde måste alltid vara slutparantes,
                 * så spolar fram en char */
                ++(*index);
                if (lisp->key_waiting_for_val) {
                    free (lisp->key_waiting_for_val);
                    lisp->key_waiting_for_val = NULL;
                }
                if (key) {
                    free (key);
                    key = NULL;
                }
                return NULL;
            case PARSE_STATE_IF:
            case PARSE_STATE_OP:
                LISP_DEBUG;
                if (current_token == LEFT_PARENTHESIS) {
                    LISP_DEBUG;
                    val_a = lisp_parse (lisp, index);
                }
                else {
                    LISP_DEBUG;
                    val_a = lisp_val_from_string (lisp, index);
                }
                parser_state = PARSE_STATE_ONE_VALUE;
#if DEBUG
                if (val_a) {
                    tmp = (int*) lisp_value_get (val_a);
                    printf ("val_a: %d\n", *tmp);
                }
#endif
                break;
            case PARSE_STATE_ONE_VALUE:
                LISP_DEBUG;
                if (current_token == LEFT_PARENTHESIS) {
                    val_b = lisp_parse (lisp, index);
                }
                else {
                    val_b = lisp_val_from_string (lisp, index);
                }
                parser_state = PARSE_STATE_TWO_VALUES;
#if DEBUG
                if (val_b) {
                    tmp = (int*) lisp_value_get (val_b);
                    printf ("val_b: %d\n", *tmp);
                }
#endif
                break;
            case PARSE_STATE_TWO_VALUES:
                LISP_DEBUG;
                if (statement == IF_STATEMENT) {
                    LISP_DEBUG;
                    if (current_token == LEFT_PARENTHESIS) {
                    LISP_DEBUG;
                        val_c = lisp_parse (lisp, index);
                    }
                    else {
                        LISP_DEBUG;
                        val_c = lisp_val_from_string (lisp, index);
                    }
                    parser_state = PARSE_STATE_THREE_VALUES;
                }
                else if (current_token == RIGHT_PARENTHESIS) {
                    LISP_DEBUG;
                    parser_state = PARSE_STATE_RIGHT_PAR;
                    goto result_no_statement;
                }
                else {
                    LISP_ERROR;
                    goto erroneous_expr;
                }
#if DEBUG
                if (val_c) {
                    tmp = (int*) lisp_value_get (val_c);
                    printf ("val_c: %d\n", *tmp);
                }
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
            case PARSE_STATE_SEQUENTIAL:
                if (current_token == RIGHT_PARENTHESIS)
                    return result;
                if (result)
                    lisp_value_free (&result);
                result = lisp_parse (lisp, index);
                break;
            default:
                LISP_ERROR;
                goto erroneous_expr;
        current_token = NO_TOKEN;
        }
    }
erroneous_expr:
    printf ("not supported (yet?)\n");
    return NULL;
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
    LISP_DEBUG;
    result = lisp_evaluate (lisp, op, val_a, val_b);
    rdt_node_add_res (lisp->current_rdt_node, result);
    rtd_node_parent = rdt_node_get_parent (lisp->current_rdt_node);
    if (rtd_node_parent)
        lisp->current_rdt_node = rtd_node_parent;

    lisp_value_unref (val_a);
    lisp_value_unref (val_b);
    lisp_value_unref (val_c);

    return result;
}

void
lisp_print_rdt_tree (Lisp * lisp) {
    rdt_node_print_tree (lisp->rdt_tree);
}

int
lisp_run_expr (Lisp * lisp, char * expr, int * error) {
    LISP_DEBUG;
    LispValue *lv = NULL;
    int *result_ptr = NULL;
    int index = 0;
    int result = 0;

    if (!lisp || !expr)
        return -1;

    if (lisp->expr)
        free (lisp->expr);

    lisp->expr = expr;
    lisp->expr_size = strlen (expr);

    lv = lisp_parse (lisp, &index);
    result_ptr = (int*) lisp_value_get (lv);
    if (result_ptr)
        result = *result_ptr;
    else
        result = -1;
    *error = lisp->error;

    lisp_value_unref (lv);

    return result;
}
