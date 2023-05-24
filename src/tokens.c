#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tokens.h"

Token
token_from_string (const char * s, const int string_size, int * index) {
    char c;
    char *token_string = 0;
    Token final_token = NO_TOKEN;

    for (;; ++(*index)) {
        c = s[*index];

        if (c == '(') {
            return LEFT_PARENTHESIS;
        }
        if (c == ')') {
            return RIGHT_PARENTHESIS;
        }
        if (c == '+') {
            return ADD;
        }
        if (c == '-') {
            return SUB;
        }
        if (c == '*') {
            return MUL;
        }
        if (c == '%') {
            return MOD;
        }
        if (c == '=') {
            return EQ;
        }
        if (c == '>') {
            if (string_size > *index && s[*index + 1] == '=') {
                ++(*index);
                return GTE;
            }
            else return GT;
        }
        if (c == '<') {
            if (string_size > *index && s[*index + 1] == '=') {
                ++(*index);
                return LTE;
            }
            else return LT;
        }

        for (int i = *index; i < string_size; ++i) {
            if (isspace (c) && i > 1) {
                token_string = (char*) calloc (i - *index + 1, sizeof (char));
                memcpy (token_string, s + *index, i - *index);
                if (strcmp (token_string, "if") == 0) {
                    final_token = IF;;
                    goto return_token;
                }
                if (strcmp (token_string, "for") == 0) {
                    final_token = FOR;
                    goto return_token;
                }
                if (strcmp (token_string, "while") == 0) {
                    final_token = WHILE;
                    goto return_token;
                }
                if (strcmp (token_string, "compose") == 0) {
                    final_token = COMPOSE;
                    goto return_token;
                }
                free (token_string);
                token_string = 0;
            }
            else {
                goto return_no_token;
            }

            if (!isalpha (c)) {
                goto return_no_token; 
            }
            
        }
    }
return_no_token:
    if (token_string)
        free (token_string);
    return NO_TOKEN;
return_token:
    if (token_string)
        free (token_string);
    return final_token;
}

bool token_is_op (Token token) {
    if (token != EQ  &&
        token != ADD &&
        token != SUB &&
        token != MUL &&
        token != MOD &&
        token != GT  &&
        token != GTE &&
        token != LT  &&
        token != LTE) {
        return FALSE;
    }
    return TRUE;
}

char *
token_as_string (Token token) {
    switch (token) {
        case NO_TOKEN:
            return "NO_TOKEN";
        case LEFT_PARENTHESIS:
            return "LEFT_PARENTHESIS";
        case RIGHT_PARENTHESIS:
            return "RIGHT_PARENTHESIS";
        case EQ:
            return "EQ";
        case ADD:
            return "ADD";
        case SUB:
            return "SUB";
        case MUL:
            return "MUL";
        case MOD:
            return "MOD";
        case GT:
            return "GT";
        case GTE:
            return "GTE";
        case LT:
            return "LT";
        case LTE:
            return "LTE";
        case IF:
            return "IF";
        case WHILE:
            return "WHILE";
        case FOR:
            return "FOR";
        case COMPOSE:
            return "COMPOSE";
    }
    return 0;
}
