#include "ocaml.h"
#include "lexer.h"
#include "lambda.h"

#ifndef PARSER_H
#define PARSER_H

ml_term_t* parser(token_t* lexed_code, int* pos, int n);

#endif // PARSER_H