#include <stdbool.h>
#include "ocaml.h"

#ifndef LEXER_H
#define LEXER_H

typedef struct trie {
    bool final;
    bool ambiguous;
    token_t read_token;
    struct trie* next[256];
} trie_t;

// Token lists
token_list_t* concat(token_t hd, token_list_t* tl);
void free_tok_list(token_list_t* lst);
int lstlen(token_list_t* lst);
token_t* lst_of_arr(token_list_t* lst, int* n); // n is modified to point to the length of the resulting table
void free_token(token_t t);
void free_token_arr(token_t* t, int n);
token_list_t* concat(token_t hd, token_list_t* tl);
token_list_t* tok_lst_init(void);
void print_token(token_t tok);
void print_token_array(token_t* t, int n);

// Trie methods
trie_t* init_trie(void);
void free_trie(trie_t* trie);
void add_word(trie_t* trie, char* w, token_t tok, int n); // WARNING : if there is a word w prefix of another word u, you should add w before u in the trie, otherwise the lexer won't work

// Lexer
token_t read_a_word(char* code, trie_t* cur, int start, int* pos);
token_t* lexer(char* code, trie_t* dict, int* n);

#endif // LEXER_H