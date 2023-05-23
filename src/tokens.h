#ifndef _TOKENS_H_
#define _TOKENS_H_

#include "utils.h"

typedef enum _token {
    NO_TOKEN,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    EQ,
    ADD,
    SUB,
    MUL,
    MOD,
    GT,
    GTE,
    LT,
    LTE,
    IF,
    WHILE,
    FOR,
    COMPOSE
} Token;

/* (sub)string that evaluates to token never contains numbers */

Token token_from_string (const char * s, const int string_size, int * index);
bool token_is_op (Token token);

#endif /* _TOKENS_H_ */
