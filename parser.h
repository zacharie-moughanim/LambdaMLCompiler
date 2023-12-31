#include "ocaml.h"
#include "lexer.h"
#include "lambda.h"

#ifndef PARSER_H
#define PARSER_H

typedef enum stop_cond {
    STOP_IN,
    STOP_SEMICOLON,
    STOP_THEN,
    STOP_END_PARENTHESIS,
    STOP_EOF
} stop_cond_t;

ml_term_t* parser(token_t* lexed_code, int* pos, int n, stop_cond_t until);

#endif // PARSER_H