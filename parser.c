#include "parser.h"
#include "ocaml.h"
#include "lexer.h"
#include "lambda.h"

void pre_process(token_t* lexed_code, int n) { // convert begin/end into (/) and let f x = x into let f = fun x -> x 
    for(int i = 0; i < n; ++i) {
        if(lexed_code[i].key == KEYWORD && lexed_code[i].val.f_val == BEGIN) {
            lexed_code[i].val.f_val = OPEN_PARENTHESIS;
        } else if(lexed_code[i].key == KEYWORD && lexed_code[i].val.f_val == END) {
            lexed_code[i].val.f_val = END_PARENTHESIS;
        }
    }
}

void incr_pos(int *pos, int n) {
    ++(*pos);
    if(*pos == n) {
        fprintf(stderr, "Syntax Error : Unexpected End of File");
    }
}

// Parse one term and then stop, for instance, doesn't read M N nor M;N nor M + N but only M
// After a call to one_parser, pos points to the position just after the last token read
ml_term_t* one_parser(token_t* lexed_code, int* pos, int n) {
    if(lexed_code[*pos].key == KEYWORD) {
        switch(lexed_code[*pos].val.f_val) {
            case LET :
                incr_pos(pos, n);
                bool is_rec = (lexed_code[*pos].key == KEYWORD && lexed_code[*pos].val.f_val == REC);
                if(is_rec) {incr_pos(pos, n);}
                ml_term_t* argument = one_parser(lexed_code, pos, n);
                if(!(lexed_code[*pos].key == KEYWORD && lexed_code[*pos].val.f_val == EQUALS)) {
                    --(*pos);
                    lexed_code[*pos]->key = FUN
                    for(int tmp_pos = (*pos); !(lexed_code[tmp_pos].key == KEYWORD && lexed_code[tmp_pos].val.f_val == EQUALS); ++tmp_pos) {}
                    lexed_code[tmp_pos].val.f_val = MAPSTO;
                    // read arg
                    ml_term_t* val = parser(lexed_code, pos, n, IN);
                    ml_term_t* in = parser(lexed_code, pos, n, SEMICOLON);
                    return Let(argument, is_rec, val, in);
                } else {
                    incr_pos(pos, n);
                    ml_term_t* val = parser(lexed_code, pos, n, IN);
                    ml_term_t* in = parser(lexed_code, pos, n, SEMICOLON);
                    return Let(argument, is_rec, val, in);
                }
            case FUN :
                incr_pos(pos, n);
                ml_term_t* argument = one_parser(lexed_code, pos, n);
                if(!(lexed_code[*pos].key == KEYWORD && lexed_code[*pos].val.f_val == MAPSTO)) {
                    --(*pos);
                    lexed_code[*pos].key = KEYwORD;
                    lexed_code[*pos].val.f_val = FUN;
                    ml_term_t* body = one_parser(lexed_code, pos, n);
                } else {
                    incr_pos(pos, n);
                    ml_term_t* body = one_parser(lexed_code, pos, n);
                }
                switch(argument->type) {
                    case VARIABLE :
                        return Fun(argument->content.var_name, body);
                    break;
                    case COUPLE : // TODO Take care of declared types
                    case LIST :
                        return Fun("v", Match("v", {argument}, {body}));
                    break;
                    default :
                        fprintf("Syntax Error : Not a valid argument");
                        return;
                }
            case TYPE :
                // TODO
            case IF :
                incr_pos(pos, n);
                ml_term_t* cond = parser(lexed_code, pos, n, THEN);
                ml_term_t* i = one_parser(lexed_code, pos, n);
                ml_term_t* e;
                if(lexed_code[*pos].key == KEYWORD && lexed_code[*pos].val.f_val == ELSE) {
                    incr_pos(pos, n);
                    e = one_parser(lexed_code, pos, n);
                } else {
                    e = ml_unit();
                }
                return IfThenElse(cond, i, e);
            case NOT :
                incr_pos(pos, n);
                ml_term_t* lhs = one_parser(lexed_code, pos, n):
                return BoolForm("NG", lhs, NULL);
            case OPEN_PARENTHESIS :
                incr_pos(pos, n);
                return parser(lexed_code, pos, n, END_PARENTHESIS);
            case OPEN_SQBRACKET :
                incr_pos(pos, n);
                if(!(lexed_code[*pos].key == KEYWORD && lexed_code[*pos].val.f_val == END_SQBRACKET)) {
                    ml_term_t* head = one_parser(lexed_code, pos, n);
                    lexed_code[*pos].key = KEYWORD;
                    lexed_code[*pos].val.f_val = OPEN_SQBRACKET;
                    return List(head, one_parser(lexed_code, pos, n));
                } else {
                    return NULL;
                }
            case OPEN_BRACKET :
                fprintf(stderr, "TODO");
            case TRUE :
                return ml_bool(true);
            case FALSE :
                return ml_bool(false);
            default :
                fprintf(stderr, "Syntax Error");
                return;
        }
    } else if(lexed_code[*pos].key == IDENTIFIER) {
        incr_pos(pos, n);
        return ml_var(lexed_code[*pos - 1].val.id_name);
    } else if(lexed_code[*pos].key == LITERAL) { // Thus, the literal is not a boolean
        incr_pos(pos, n);
        return ml_int(lexed_code[*pos - 1].val.n);
    }
}

ml_term_t* merge_terms(ml_term_t* M, token_t* lexed_code, int* pos, int n) {
    if(!lexed_code[*pos].is_finite) {
        fprintf(stderr, "Syntax Error : Not a valid operator");
        return NULL;
    }
    switch(lexed_code[*pos].val.f_val) {
        case LESS :
        case GREATER :
        case EQUALS :
        case PLUS :
        case MINUS :
        case TIMES :
        case DIVIDE :
        case AND :
        case OR :
        case DOT :
        case COMMA :
        case COLON :
        case SEMICOLON :
    }
}

// For [until] possible values, see parser.h
ml_term_t* parser(token_t* lexed_code, int* pos, int n, stop_cond_t until) {
    // We can assume that *pos < n, because if there are equal, it was already detected by the incr_pos function
    ml_term_t* M = one_parser(lexed_code, pos, n);
    switch(until) {
        case IN :
            if(!(lexed_code[*pos].is_finite && lexed_code[*pos].val.f_val == IN)) {
                merge_terms(M, lexed_code, pos, n)
            }
        case SEMICOLON :
        case THEN :
        case END_PARENTHESIS :
    }
}