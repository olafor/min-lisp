#ifndef _TOKENS_H_
#define _TOKENS_H_

#include "utils.h"

typedef enum _token {
    NO_TOKEN,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    ADD,
    COMPOSE,
    DEFVAR,
    EQ,
    FOR,
    GT,
    GTE,
    IF,
    LT,
    LTE,
    MOD,
    MUL,
    QUOTE,
    SUB,
    WHILE
} Token;

/* (sub)string that evaluates to token never contains numbers */

Token token_from_string (const char * s, const int string_size, int * index);
bool token_is_op (Token token);
char *token_as_string (Token token);

#endif /* _TOKENS_H_ */
