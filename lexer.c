#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ocaml.h"
#include "lexer.h"

// Token list methods

token_list_t* tok_lst_init(void) { return NULL; }

token_list_t* concat(token_t hd, token_list_t* tl) {
    token_list_t* n_l = malloc(sizeof(token_list_t));
    n_l->hd = hd;
    n_l->tl = tl;
    return n_l;
}

void free_tok_list(token_list_t* lst) {
    if(lst != NULL) {
        free_tok_list(lst->tl);
        free(lst);
    }
}

int lstlen(token_list_t* lst) {
    if(lst == NULL) {
        return 0;
    } else {
        return 1 + lstlen(lst->tl);
    }
}

token_t* lst_of_arr(token_list_t* lst, int* n) {
    *n = lstlen(lst);
    token_t* arr = malloc(*n * sizeof(token_t));
    token_list_t* cur = lst;
    for(int i = 0; i < *n; ++i) {
        arr[i] = cur->hd;
        cur = cur->tl;
    }
    return arr;
}

void free_token(token_t tok) {
    if(tok.key == IDENTIFIER) {
        free(tok.val.id_name);
    }
}

void free_token_arr(token_t* t, int n) {
    for(int i = 0; i < n; ++i) {
        free_token(t[i]);
    }
    free(t);
}

// Display function

void print_token(token_t tok) {
    char* key_s;
    char* val_s;
    if(tok.key == IDENTIFIER) {
        fprintf(stderr, "(%s,%s)", "IDENTIFIER", tok.val.id_name);
    } else {
        switch(tok.key) {
            case IDENTIFIER :
            key_s = "IDENTIFIER";
            break;
            case KEYWORD :
            key_s = "KEYWORD";
            break;
            case OPERATOR :
            key_s = "OPERATOR";
            break;
            case LITERAL :
            key_s = "LITERAL";
            break;
            case PUNCTUATION :
            key_s = "PUNCTUATION";
            break;
        }
        if(tok.is_finite) {
            switch(tok.val.f_val) {
                case PIPE :
                    val_s = "PIPE";
                    break;
                case MATCH :
                    val_s = "MATCH";
                    break;
                case WITH :
                    val_s = "WITH";
                    break;
                case LET :
                    val_s = "LET";
                    break;
                case GEQ :
                    val_s = "GEQ";
                    break;
                case LEQ :
                    val_s = "LEQ";
                    break;
                case NEQ :
                    val_s = "NEQ";
                    break;
                case CONCAT_LST :
                    val_s = "CONCAT_LST";
                    break;
                case CONCAT_STR :
                    val_s = "CONCAT_STR";
                    break;
                case CONS :
                    val_s = "CONS";
                    break;
                case REC :
                    val_s = "REC";
                    break;
                case FUN :
                    val_s = "FUN";
                    break;
                case FUNMATCH :
                    val_s = "FUNMATCH";
                    break;
                case MAPSTO :
                    val_s = "MAPSTO";
                    break;
                case TYPE :
                    val_s = "TYPE";
                    break;
                case IN :
                    val_s = "IN";
                    break;
                case EQUALS :
                    val_s = "EQUALS";
                    break;
                case PLUS :
                    val_s = "PLUS";
                    break;
                case MINUS :
                    val_s = "MINUS";
                    break;
                case TIMES :
                    val_s = "TIMES";
                    break;
                case DIVIDE :
                    val_s = "DIVIDE";
                    break;
                case AND :
                    val_s = "AND";
                    break;
                case OR :
                    val_s = "OR";
                    break;
                case NOT :
                    val_s = "NOT";
                    break;
                case QUOTE :
                    val_s = "QUOTE";
                    break;
                case BEGIN :
                    val_s = "BEGIN";
                    break;
                case END :
                    val_s = "END";
                    break;
                case OPEN_PARENTHESIS :
                    val_s = "OPEN_PARENTHESIS";
                    break;
                case END_PARENTHESIS :
                    val_s = "END_PARENTHESIS";
                    break;
                case OPEN_SQBRACKET :
                    val_s = "OPEN_SQBRACKET";
                    break;
                case END_SQBRACKET :
                    val_s = "END_SQBRACKET";
                    break;
                case OPEN_BRACKET :
                    val_s = "OPEN_BRACKET";
                    break;
                case END_BRACKET :
                    val_s = "END_BRACKET";
                    break;
                case OPEN_COMMENT :
                    val_s = "OPEN_COMMENT";
                    break;
                case END_COMMENT :
                    val_s = "END_COMMENT";
                    break;
                case DOT :
                    val_s = "DOT";
                    break;
                case COMMA :
                    val_s = "COMMA";
                    break;
                case COLON :
                    val_s = "COLON";
                    break;
                case SEMICOLON :
                    val_s = "SEMICOLON";
                    break;
                case READING_ERROR :
                    val_s = "READING_ERROR";
                break;
                case TRUE :
                    val_s = "TRUE";
                break;
                case FALSE :
                    val_s = "FALSE";
                break;
                case LESS :
                    val_s = "LESS";
                break;
                case GREATER :
                    val_s = "GREATER";
                break;
                case IF :
                    val_s = "IF";
                break;
                case THEN :
                    val_s = "THEN";
                break;
                case ELSE :
                    val_s = "ELSE";
                break;
            }
            fprintf(stderr, "(%s,%s)",key_s,val_s);
        } else {
            fprintf(stderr, "(%s,%d)",key_s, tok.val.n);
        }
    }
}

void print_token_array(token_t* t, int n) {
    printf("[");
    for(int i = 0; i < n; ++i) {
        print_token(t[i]);
        fprintf(stderr, ";");
    }
    printf("]");
    printf("\n");
}

// Trie methods

trie_t* init_trie(void) {
    trie_t* n_trie = malloc(sizeof(trie_t));
    n_trie->final = false;
    n_trie->ambiguous = false;
    for(int i = 0; i < 256; ++i) {
        n_trie->next[i] = NULL;
    }
    return n_trie;
}

void free_trie(trie_t* trie) {
    if(trie != NULL) {
        for(int i = 0; i < 256; ++i) {
            free_trie(trie->next[i]);
        }
        free(trie);
    }
}

void add_word(trie_t* trie, char* w, token_t tok, int i) {
    if(w[i] == '\0') {
        trie->final = true;
        trie->read_token = tok;
    } else {
        if(trie->next[(int)w[i] + 128] == NULL) {
            trie->next[(int)w[i] + 128] = init_trie();
        }
        if(trie->final) {
            trie->ambiguous = true;
        }
        add_word(trie->next[(int)w[i] + 128], w, tok, i + 1);
    }
}

// Lexer

token_t read_a_word(char* code, trie_t* cur, int start, int* pos) {
    switch(code[*pos]) {
        case '\0' :
        case ' ' :
        case '\n' :
        case '.' :
        case ',' :
        case ';' :
        case ':' :
        case '<' :
        case '>' :
        case '(' :
        case ')' :
        case '[' :
        case ']' :
        case '{' :
        case '}' :
        case '!' :
        case '=' :
        case '+' :
        case '-' :
        case '*' :
        case '/' :
        case '&' :
        case '|' :
            if(cur == NULL) { // Then, start != *pos is true.
                char* id = malloc((*pos + 1 - start)*sizeof(char));
                for(int i = start; i < *pos; ++i) {
                    id[i - start] = code[i];
                }
                id[*pos - start] = '\0';
                token_t tok = {.key = IDENTIFIER, .val = {.id_name = id}};
                return tok;
            } else {
                if(!(cur->final) && start != *pos) {
                    char* id = malloc((*pos + 1 - start)*sizeof(char));
                    for(int i = start; i < *pos; ++i) {
                        id[i - start] = code[i];
                    }
                    id[*pos - start] = '\0';
                    token_t tok = {.key = IDENTIFIER, .val = {.id_name = id}};
                    return tok;
                } else if (!(cur->final) && start == *pos) {
                    return read_a_word(code, cur->next[(int)code[(*pos)++] + 128], start, pos);
                } else {
                    if(cur->ambiguous) {
                        int* tmp_pos = malloc(sizeof(int));
                        *tmp_pos = *pos;
                        token_t ambiguity_test = read_a_word(code, cur->next[(int)code[(*tmp_pos)++] + 128], start, tmp_pos);
                        switch(ambiguity_test.key) {
                            case IDENTIFIER :
                                free(tmp_pos);
                                return cur->read_token;
                            break;
                            default :
                                *pos = *tmp_pos;
                                free(tmp_pos);
                                return ambiguity_test;
                            break;
                        }
                    } else {
                        return cur->read_token;
                    }
                }
            }
        break;
        default :
            if(cur == NULL) {
                ++(*pos);
                return read_a_word(code, NULL, start, pos);
            } else {
                return read_a_word(code, cur->next[(int)code[(*pos)++] + 128], start, pos);
            }
        break;
    }
}

token_list_t* lexer_list(char* code, trie_t* dict, int pos) { // FIXME not do that : {reading '*x' as (IDENTIFIER, *x)}
    // Eliminating unused characters
    while(code[pos] == ' ' || code[pos] == '\n') {
        ++pos;
    }

    // Eliminating comments
    if(code[pos] == '(' && code[pos + 1] == '*') {
        ++pos;
        while(!(code[pos] == '*' && code[pos + 1] == ')') && code[pos+1] != '\0') ++pos;
        if(code[pos + 1] == '\0') {
            fprintf(stderr, "Unmatched open comment\n");
            return NULL;
        } else {
            pos += 2; // Well parenthesized comment
            return lexer_list(code, dict, pos);
        }
    }
    if(code[pos] == '*' && code[pos + 1] ==   ')') {
        fprintf(stderr, "Unmatched closed comment\n");
        return NULL;
    }

    // Reading an integer
    if('0' <= code[pos] && code[pos] <= '9') {
        int n = 0;
        int end = pos;
        while('0' <= code[end] && code[end] <= '9') ++end;
        while('0' <= code[pos] && code[pos] <= '9') {
            n += (((int)code[pos]) - '0') * ((int)pow(10, end - 1 - pos));
            ++pos;
        }
        token_t cur_word = {.key = LITERAL, .is_finite = false, .val = {n}};
        return concat(cur_word, lexer_list(code, dict, pos));
    }

    // Reading a built-in word
    if(code[pos] == '\0') {
        return NULL;
    } else {
        token_t cur_word = read_a_word(code, dict, pos, &pos);
        return concat(cur_word, lexer_list(code, dict, pos));
    }
}

token_t* lexer(char* code, trie_t* dict, int* n) {
    token_list_t* lexed_lst = lexer_list(code, dict, 0);
    token_t* lexed_code = lst_of_arr(lexed_lst, n);
    free_tok_list(lexed_lst);
    return lexed_code;
}