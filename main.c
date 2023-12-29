#define N_WORDS 37

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ocaml.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char** argv) {
    trie_t* ml_dict = init_trie();
    /* Ambiguous words :
    - and ->
    : and :=
    */
    char* ml_word[N_WORDS] = {
    "=",
    "+",
    "-",
    "*",
    "/",
    "!",
    "&&",
    "||",
    "not",
    "<",
    ">",
    "if",
    "then",
    "else",
    "let",
    "rec",
    "fun",
    "->",
    "type",
    "in", // Add while, for, done
    "\"",
    "begin", "end",
    "(", ")",
    "[", "]",
    "{", "}",
    ".",
    ",",
    ":",
    ":=",
    "<-",
    ";",
    "true",
    "false"};
    token_t ml_token[N_WORDS] = {
    {.key = OPERATOR, .is_finite = true, .val = {EQUALS}},
    {.key = OPERATOR, .is_finite = true, .val = {PLUS}},
    {.key = OPERATOR, .is_finite = true, .val = {MINUS}},
    {.key = OPERATOR, .is_finite = true, .val = {TIMES}},
    {.key = OPERATOR, .is_finite = true, .val = {DIVIDE}},
    {.key = OPERATOR, .is_finite = true, .val = {DEREF}},
    {.key = OPERATOR, .is_finite = true, .val = {AND}},
    {.key = OPERATOR, .is_finite = true, .val = {OR}},
    {.key = OPERATOR, .is_finite = true, .val = {NOT}},
    {.key = OPERATOR, .is_finite = true, .val = {LESS}},
    {.key = OPERATOR, .is_finite = true, .val = {GREATER}},
    {.key = KEYWORD, .is_finite = true, .val = {IF}},
    {.key = KEYWORD, .is_finite = true, .val = {THEN}},
    {.key = KEYWORD, .is_finite = true, .val = {ELSE}},
    {.key = KEYWORD, .is_finite = true, .val = {LET}},
    {.key = KEYWORD, .is_finite = true, .val = {REC}},
    {.key = KEYWORD, .is_finite = true, .val = {FUN}},
    {.key = KEYWORD, .is_finite = true, .val = {MAPSTO}},
    {.key = KEYWORD, .is_finite = true, .val = {TYPE}},
    {.key = KEYWORD, .is_finite = true, .val = {IN}},
    {.key = PUNCTUATION, .is_finite = true, .val = {QUOTE}},
    {.key = PUNCTUATION, .is_finite = true, .val = {BEGIN}},
    {.key = PUNCTUATION, .is_finite = true, .val = {END}},
    {.key = PUNCTUATION, .is_finite = true, .val = {OPEN_PARENTHESIS}},
    {.key = PUNCTUATION, .is_finite = true, .val = {END_PARENTHESIS}},
    {.key = PUNCTUATION, .is_finite = true, .val = {OPEN_SQBRACKET}},
    {.key = PUNCTUATION, .is_finite = true, .val = {END_SQBRACKET}},
    {.key = PUNCTUATION, .is_finite = true, .val = {OPEN_BRACKET}},
    {.key = PUNCTUATION, .is_finite = true, .val = {END_BRACKET}},
    {.key = PUNCTUATION, .is_finite = true, .val = {DOT}},
    {.key = PUNCTUATION, .is_finite = true, .val = {COMMA}},
    {.key = PUNCTUATION, .is_finite = true, .val = {COLON}},
    {.key = OPERATOR, .is_finite = true, .val = {REFGETS}},
    {.key = OPERATOR, .is_finite = true, .val = {GETS}},
    {.key = PUNCTUATION, .is_finite = true, .val = {SEMICOLON}},
    {.key = LITERAL, .is_finite = true, .val = {TRUE}},
    {.key = LITERAL, .is_finite = true, .val = {FALSE}}
    };

    for(int i = 0; i < N_WORDS; ++i) {
        add_word(ml_dict, ml_word[i], ml_token[i], 0);
    }
    if(argc >= 1) {
        int n = 0;
        token_t* lexed = lexer(argv[1], ml_dict, &n);
        print_token_array(lexed, n);
        free_trie(ml_dict);
        int i = 0;
        ml_term_t* parsed = parser(lexed, &i, n);
        /*int i = 0;
        ml_term_t* parsed = read_argument(lexed, &i, 1);*/
        free_token_arr(lexed, n);
        free_ml_term(parsed);
    }
    return 0;
}