#include "parser.h"
#include "ocaml.h"
#include "lexer.h"
#include "lambda.h"

bool precedence(token_t op, int* prec) {
    // After this call,         prec points to the precedence value
    // And the function returns +1/0/-1 for left_assoc/suffix_or_prefix op/right_assoc respectively
    int precedence;
    if(!lexed_code[*pos].is_finite) {
        // Hence, it is an application : M N
        precedence = 10;
        return 1;
    }
    switch(lexed_code[*pos].val.f_val) {
        case LESS :
            precedence = 4;
            return 1;
        case GREATER :
            precedence = 4;
            return 1;
        case LEQ :
            precedence = 4;
            return 1;
        case QEG :
            precedence = 4;
            return 1;
        case NEQ :
            precedence = 4;
            return 1;
        case EQUALS :
            precedence = 4;
            return 1;
        case PLUS :
            precedence = 7;
            return 1;
        case MINUS :
            precedence = 7;
            return 1;
        case TIMES :
            precedence = 8;
            return 1;
        case DIVIDE :
            precedence = 8;
            return 1;
        case AND :
            precedence = 3;
            return 1;
        case OR :
            precedence = 2;
            return 1;
        case DOT : // TODO : is this a relevant case ?
        case COMMA :
            precedence = 1;
            return 0;
        case COLON :
            return NULL;
        case SEMICOLON :
            precedence = 0;
            return -1;
        case CONCAT_LST :
            precedence = 5;
            return -1;
        case CONCAT_STR :
            precedence = 5;
            return -1;
        case CONS :
            precedence = 6;
            return -1;
        default : // Hence, it is an application : M N
            precedence = 10;
            return 1;
    }
}

int PrecedenceLt(token_finite_value_t op1, token_finite_value_t op2) {
    int prec1;
    int prec2;
    int assoc1 = precedence(op1, &prec1);
    int assoc2 = precedence(op2, &prec2);
    if(prec1 < prec2) {
        return 1;
    } else if (prec1 == prec2 && assoc2 == -1) {
        return 0;
    } else {
        return -1;
    }
}

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
                    ml_term_t* val = parser(lexed_code, pos, n, STOP_IN);
                    ml_term_t* in = parser(lexed_code, pos, n, STOP_SEMICOLON);
                    return Let(argument, is_rec, val, in);
                } else {
                    incr_pos(pos, n);
                    ml_term_t* val = parser(lexed_code, pos, n, STOP_IN);
                    ml_term_t* in = parser(lexed_code, pos, n, STOP_SEMICOLON);
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
                        return NULL;
                }
            case TYPE :
                // TODO
            case IF :
                incr_pos(pos, n);
                ml_term_t* cond = parser(lexed_code, pos, n, STOP_THEN);
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
                return parser(lexed_code, pos, n, STOP_END_PARENTHESIS);
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
                return NULL;
        }
    } else if(lexed_code[*pos].key == IDENTIFIER) {
        incr_pos(pos, n);
        return ml_var(lexed_code[*pos - 1].val.id_name);
    } else if(lexed_code[*pos].key == LITERAL) { // Thus, the literal is not a boolean
        incr_pos(pos, n);
        return ml_int(lexed_code[*pos - 1].val.n);
    }
}

ml_term_t* merge_terms(ml_term_t* M, int min_precedence, token_t* lexed_code, int* pos, int n) {
    token_t op = lexed_code[*pos];
    token_t next_op = lexed_code[*pos];

    int precedence_comparison;
    int cur_precedence = precedence(next_op);

    ml_term_t* N;
    while(min_precedence <= cur_precedence && *pos < n) {
        op = next_op;
        if(precedence != 10) { incr_pos(pos, n); }
        N = one_parser(lexed_code, pos, n);
        next_op = lexed_code[*pos];
        precedence_comparison = PrecedenceLt(op, next_op);
        while(0 <= precedence_comparison && *pos < n) {
            N = parse_expression(N, min_precedence + precedence_comparison, lexed_code, pos, n);
            next_op = lexed_code[*pos];
        }
        M = ml_constructor(op, M, N);
    }
    return M;
}

// For [until] possible values, see parser.h
ml_term_t* parser(token_t* lexed_code, int* pos, int n, stop_cond_t until) {
    // We can assume that *pos < n, because if they are equal, it was already detected by the incr_pos function
    ml_term_t* M = one_parser(lexed_code, pos, n);
    switch(until) {
        case STOP_IN :
            while(!(lexed_code[*pos].is_finite && lexed_code[*pos].val.f_val == IN)) {
                M = merge_terms(M, 0, lexed_code, pos, n);
            }
        case STOP_SEMICOLON :
            while(!(lexed_code[*pos].is_finite && lexed_code[*pos].val.f_val == SEMICOLON) && *pos < n) {
                M = merge_terms(M, 0, lexed_code, pos, n);
            }
        case STOP_THEN :
            while(!(lexed_code[*pos].is_finite && lexed_code[*pos].val.f_val == THEN)) {
                M = merge_terms(M, 0, lexed_code, pos, n);
            }
        case STOP_END_PARENTHESIS :
            while(!(lexed_code[*pos].is_finite && lexed_code[*pos].val.f_val == END_PARENTHESIS)) {
                M = merge_terms(M, 0, lexed_code, pos, n);
            }
    }
}