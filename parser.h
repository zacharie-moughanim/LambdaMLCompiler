#include "ocaml.h"
#include "lexer.h"
#include "lambda.h"

#ifndef PARSER_H
#define PARSER_H

typedef enum stop_cond {
    IN,
    SEMICOLON,
    THEN,
    END_PARENTHESIS
} stop_cond_t;

ml_term_t* parser(token_t* lexed_code, int* pos, int n);

#endif // PARSER_H