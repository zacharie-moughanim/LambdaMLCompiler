#include "parser.h"
#include "ocaml.h"
#include "lexer.h"
#include "lambda.h"

bool precedence(token_t op, int* prec) {
    // After this call,         prec points to the precedence value
    // And the function returns +1/0/-1 for left_assoc/suffix_or_prefix op/right_assoc respectively
    if(!op.is_finite) {
        // Hence, it is an application : M N
        *prec = 10;
        return 1;
    }
    switch(op.val.f_val) {
        case LESS :
            *prec = 4;
            return 1;
        case GREATER :
            *prec = 4;
            return 1;
        case LEQ :
            *prec = 4;
            return 1;
        case GEQ :
            *prec = 4;
            return 1;
        case NEQ :
            *prec = 4;
            return 1;
        case EQUALS :
            *prec = 4;
            return 1;
        case PLUS :
            *prec = 7;
            return 1;
        case MINUS :
            *prec = 7;
            return 1;
        case TIMES :
            *prec = 8;
            return 1;
        case DIVIDE :
            *prec = 8;
            return 1;
        case AND :
            *prec = 3;
            return 1;
        case OR :
            *prec = 2;
            return 1;
        case DOT : // TODO : is this a relevant case ?
        case COMMA :
            *prec = 1;
            return 0;
        case COLON :
            return NULL;
        case SEMICOLON :
            *prec = 0;
            return -1;
        case CONCAT_LST :
            *prec = 5;
            return -1;
        case CONCAT_STR :
            *prec = 5;
            return -1;
        case CONS :
            *prec = 6;
            return -1;
        default : // Hence, it is an application : M N
            *prec = 10;
            return 1;
    }
}

int PrecedenceLt(token_t op1, token_t op2) {
    int prec1;
    int prec2;
    precedence(op1, &prec1);
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

void incr_pos(token_t* lexed_code, int *pos, int n) {
    fprintf(stderr, "\n");
    print_token(lexed_code[*pos]);
    ++(*pos);
    if(n < *pos) { // We let *pos reach n, because of the invariant '[lexed_code[*pos]] is the token just after the one just read'
        fprintf(stderr, "Syntax Error : Unexpected End of File");
    } else {
        if(n == *pos) {
            fprintf(stderr, "WARNING :EOF\n");
        } else {
            print_token(lexed_code[*pos]);
        }
        fprintf(stderr, "\n");
    }
}

// Parse one term and then stop, for instance, doesn't read M N nor M;N nor M + N but only M
// After a call to one_parser, pos points to the position just after the last token read
ml_term_t* one_parser(token_t* lexed_code, int* pos, int n) {
    if(lexed_code[*pos].key == KEYWORD || lexed_code[*pos].key == OPERATOR || lexed_code[*pos].key == PUNCTUATION) {
        ml_term_t* argument;
        switch(lexed_code[*pos].val.f_val) {
            case LET :
                incr_pos(lexed_code, pos, n);
                bool is_rec = (lexed_code[*pos].key == KEYWORD && lexed_code[*pos].val.f_val == REC);
                if(is_rec) { incr_pos(lexed_code, pos, n); }
                fprintf(stderr, "HEY\n");
                argument = one_parser(lexed_code, pos, n);
                fprintf(stderr, "HEY\n");
                if(!(lexed_code[*pos].key == OPERATOR && lexed_code[*pos].val.f_val == EQUALS)) {
                    fprintf(stderr, "BONJOUR\n");
                    --(*pos);
                    lexed_code[*pos].key = KEYWORD;
                    lexed_code[*pos].val.f_val = FUN;
                    int tmp_pos;
                    for(tmp_pos = (*pos); !(lexed_code[tmp_pos].key == OPERATOR && lexed_code[tmp_pos].val.f_val == EQUALS); ++tmp_pos) {}
                    lexed_code[tmp_pos].val.f_val = MAPSTO;
                    // read arg
                    ml_term_t* val = parser(lexed_code, pos, n, STOP_IN);
                    ml_term_t* in = parser(lexed_code, pos, n, STOP_SEMICOLON);
                    return Let(argument, is_rec, val, in);
                } else {
                    fprintf(stderr, "BONSOIR\n");
                    incr_pos(lexed_code, pos, n);
                    ml_term_t* val = parser(lexed_code, pos, n, STOP_IN);
                    ml_term_t* in = parser(lexed_code, pos, n, STOP_SEMICOLON);
                    return Let(argument, is_rec, val, in);
                }
            case FUN :
                incr_pos(lexed_code, pos, n);
                argument = one_parser(lexed_code, pos, n);
                ml_term_t* body;
                if(!(lexed_code[*pos].key == KEYWORD && lexed_code[*pos].val.f_val == MAPSTO)) {
                    --(*pos);
                    lexed_code[*pos].key = KEYWORD;
                    lexed_code[*pos].val.f_val = FUN;
                    body = one_parser(lexed_code, pos, n);
                } else {
                    incr_pos(lexed_code, pos, n);
                    body = one_parser(lexed_code, pos, n);
                }
                switch(argument->type) {
                    case VARIABLE :
                        return Fun(argument->content.var_name, body);
                    break;
                    case COUPLE : // TODO Take care of declared types
                    case LIST :
                        ml_term_t* args[1] = {argument};
                        ml_term_t* bodys[1] = {body};
                        ml_term_t* val = malloc(sizeof(ml_term_t));
                        val->type = VARIABLE;
                        val->content.var_name = "v";
                        return Fun("v", Match(val, args, bodys));
                    break;
                    default :
                        fprintf(stderr, "Syntax Error : Not a valid argument");
                        return NULL;
                }
            case TYPE :
                // TODO
                return NULL;
            case IF :
                incr_pos(lexed_code, pos, n);
                ml_term_t* cond = parser(lexed_code, pos, n, STOP_THEN);
                ml_term_t* i = one_parser(lexed_code, pos, n);
                ml_term_t* e;
                if(lexed_code[*pos].key == KEYWORD && lexed_code[*pos].val.f_val == ELSE) {
                    incr_pos(lexed_code, pos, n);
                    e = one_parser(lexed_code, pos, n);
                } else {
                    e = ml_unit();
                }
                return IfThenElse(cond, i, e);
            case NOT :
                incr_pos(lexed_code, pos, n);
                ml_term_t* lhs = one_parser(lexed_code, pos, n);
                return BoolForm('!', lhs, NULL);
            case OPEN_PARENTHESIS :
                incr_pos(lexed_code, pos, n);
                return parser(lexed_code, pos, n, STOP_END_PARENTHESIS);
            case OPEN_SQBRACKET :
                incr_pos(lexed_code, pos, n);
                if(!(lexed_code[*pos].key == PUNCTUATION && lexed_code[*pos].val.f_val == END_SQBRACKET)) {
                    ml_term_t* head = one_parser(lexed_code, pos, n);
                    lexed_code[*pos].key = KEYWORD;
                    lexed_code[*pos].val.f_val = OPEN_SQBRACKET;
                    return List(head, one_parser(lexed_code, pos, n));
                } else {
                    return NULL;
                }
            case OPEN_BRACKET :
                fprintf(stderr, "TODO");
                return NULL;
            case TRUE :
                return ml_bool(true);
            case FALSE :
                return ml_bool(false);
            default :
                fprintf(stderr, "Syntax Error");
                while(true) {}
                return NULL;
        }
    } else if(lexed_code[*pos].key == IDENTIFIER) {
        incr_pos(lexed_code, pos, n);
        return ml_var(lexed_code[*pos - 1].val.id_name);
    } else if(lexed_code[*pos].key == LITERAL) { // Thus, the literal is not a boolean
        incr_pos(lexed_code, pos, n);
        return ml_int(lexed_code[*pos - 1].val.n);
    }
    fprintf(stderr, "Go see end of parse_one function");
    return NULL;
}

bool end_condition(stop_cond_t until, token_t* lexed_code, int* pos, int n) {
    switch(until) {
        case STOP_IN :
            fprintf(stderr, "1\n");
            return lexed_code[*pos].is_finite && lexed_code[*pos].val.f_val == IN;
        case STOP_SEMICOLON :
            fprintf(stderr, "2\n");
            printf("%d..%d..%d..\n", *pos < n, true, true);
            return *pos < n ? lexed_code[*pos].is_finite && lexed_code[*pos].val.f_val == SEMICOLON : true;
        case STOP_THEN :
            fprintf(stderr, "3\n");
            return lexed_code[*pos].is_finite && lexed_code[*pos].val.f_val == THEN;
        case STOP_END_PARENTHESIS :
            fprintf(stderr, "4\n");
            return lexed_code[*pos].is_finite && lexed_code[*pos].val.f_val == END_PARENTHESIS;
        case STOP_EOF :
            fprintf(stderr, "5\n");
            return !(*pos < n);
    }
    return NULL;
}

ml_term_t* merge_terms(ml_term_t* M, int min_precedence, stop_cond_t until, token_t* lexed_code, int* pos, int n) {
    token_t op = lexed_code[*pos];
    token_t next_op = lexed_code[*pos];

    int precedence_comparison;
    int cur_precedence;
    precedence(next_op, &cur_precedence);

    ml_term_t* N;
    while(min_precedence <= cur_precedence && !end_condition(until, lexed_code, pos, n)) {
        fprintf(stderr, "hey0\n");
        op = next_op;
        if(cur_precedence != 10) { incr_pos(lexed_code, pos, n); } // i.e. if it's not a function application
        N = one_parser(lexed_code, pos, n);
        if(end_condition(until, lexed_code, pos, n)) {
            fprintf(stderr, "hey1\n");
        } else {
            fprintf(stderr, "hey1.5\n");
            next_op = lexed_code[*pos];
            precedence_comparison = PrecedenceLt(op, next_op);
        }
        while(0 <= precedence_comparison && !end_condition(until, lexed_code, pos, n)) {
            fprintf(stderr, "hey2\n");
            N = merge_terms(N, min_precedence + precedence_comparison, until, lexed_code, pos, n);
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
    fprintf(stderr,"bonjour\n");
    if(!end_condition(until, lexed_code, pos, n)) {
        fprintf(stderr, "au revoir\n");
        M = merge_terms(M, 0, until, lexed_code, pos, n);
    }
    if(!(*pos == n && (until == STOP_EOF || until == STOP_SEMICOLON))) { incr_pos(lexed_code, pos, n); }
    return M;
}