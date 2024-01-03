#include <stdlib.h>
#include <stdio.h>
#include "ocaml.h"

// Linked Lists

void cons(ml_term_t* n_head, ml_term_lst_t** lst) {
    ml_term_lst_t* n_lst = malloc(sizeof(ml_term_lst_t));
    n_lst->hd = n_head;
    n_lst->tl = (*lst);
    (*lst) = n_lst;
}

void free_ml_lst(ml_term_lst_t* lst) {
    if(lst != NULL) {
        free_ml_lst(lst->tl);
        free_ml_term(lst->hd);
        free(lst);
    }
}

// Term Constructors

ml_term_t* Match(ml_term_t* term, ml_term_t** patterns, ml_term_t** bodys) {
    ml_term_t* res = malloc(sizeof(ml_term_t)); // FIXME array copy
    res->content.match.term = term;
    res->content.match.patterns = patterns;
    res->content.match.bodys = bodys;
    return res;
}

ml_term_t* Let(ml_term_t* argument, bool is_rec, ml_term_t* val, ml_term_t* in) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = DECLARE;
    res->content.declare.is_rec = is_rec;
    switch(argument->type) {
        case VARIABLE :
            res->content.declare.var_name = argument->content.var_name;
            res->content.declare.val = val;
            res->content.declare.in = in;
        break;
        case COUPLE :
            if(is_rec) {
                fprintf(stderr, "Error : Only variables are allowed as left-hand side of 'let rec'");
                return NULL;
            }
            if(argument->content.cpl.fst->type != VARIABLE) {
                fprintf(stderr, "Syntax Error : Expected an identifier");
            } else {
                res->content.declare.var_name = argument->content.cpl.fst->content.var_name;
                if(val->type != COUPLE) {
                    fprintf(stderr, "Syntax Error : Expected a tuple to assign");
                } else {
                    res->content.declare.val = val->content.cpl.fst;
                    res->content.declare.in = Let(argument->content.cpl.snd, false, val->content.cpl.snd, in);
                }
            }
        break;    
        case LIST :
            if(is_rec) {
                fprintf(stderr, "Error : Only variables are allowed as left-hand side of 'let rec'");
                return NULL;
            }
            if(argument->content.lst.hd->type != VARIABLE) {
                fprintf(stderr, "Syntax Error : Expected an identifier");
            } else {
                res->content.declare.var_name = argument->content.lst.hd->content.var_name;
                if(val->type != LIST) {
                    fprintf(stderr, "Syntax Error : Expected a tuple to assign");
                } else {
                    res->content.declare.val = val->content.lst.hd;
                    res->content.declare.in = Let(argument->content.lst.tl, false, val->content.lst.tl, in);
                }
            }
        break;
        default :
            fprintf(stderr, "Syntax Error : Not a valid argument");
            return NULL;
    }
    return res;
}    

ml_term_t* Applml(ml_term_t* applying, ml_term_t* to) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = APPLML;
    res->content.appl.applying = applying;
    res->content.appl.to = to;
    return res;
}    

ml_term_t* Fun(char* var_name, ml_term_t* body) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = FUNC;
    res->content.func.var = var_name;
    res->content.func.body = body;
    return res;
}    

ml_term_t* Cpl(ml_term_t* fst, ml_term_t* snd) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = COUPLE;
    res->content.cpl.fst = fst;
    res->content.cpl.snd = snd;
    return res;
}     

ml_term_t* List(ml_term_t* hd, ml_term_t* tl) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = LIST;
    res->content.lst.hd = hd;
    res->content.lst.tl = tl;
    return res;
}

ml_term_t* ConcatStr(ml_term_t* str1, ml_term_t* str2) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = CONCAT_STRING;
    res->content.concat_str.str1 = str1;
    res->content.concat_str.str2 = str2;
    return res;
}

ml_term_t* ConcatLst(ml_term_t* lst1, ml_term_t* lst2) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = CONCAT_LIST;
    res->content.concat_lst.lst1 = lst1;
    res->content.concat_lst.lst2 = lst2;
    return res;
}

ml_term_t* ArithForm(char operator, ml_term_t* lhs, ml_term_t* rhs) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = ARITHM_FORMULA;
    res->content.a_form.operator = operator;
    res->content.a_form.lhs = lhs;
    res->content.a_form.rhs = rhs;
    return res;
}    

ml_term_t* BoolForm(char operator, ml_term_t* lhs, ml_term_t* rhs) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = BOOL_FORMULA;
    res->content.b_form.operator = operator;
    res->content.b_form.lhs = lhs;
    res->content.b_form.rhs = rhs;
    return res;
}       

ml_term_t* Comparison(char comparator[2], ml_term_t* lhs, ml_term_t* rhs) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = COMPARISON;
    res->content.compare.operator[0] = comparator[0];
    res->content.compare.operator[1] = comparator[1];
    res->content.compare.lhs = lhs;
    res->content.compare.rhs = rhs;
    return res;
}

ml_term_t* IfThenElse(ml_term_t* cond, ml_term_t* i, ml_term_t* e) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = CONDITION;
    res->content.ite.condition = cond;
    res->content.ite.body_i = i;
    res->content.ite.body_e = e;
    return res;
}

ml_term_t* ml_var(char* var_name) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = VARIABLE;
    res->content.var_name = var_name;
    return res;
}

ml_term_t* ml_int(int n) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = CONST_INT;
    res->content.n = n;
    return res;
}

ml_term_t* ml_bool(bool b) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = CONST_BOOL;
    res->content.b = b;
    return res;
}

ml_term_t* ml_unit(void) {
    ml_term_t* res = malloc(sizeof(ml_term_t));
    res->type = CONST_UNIT;
    return res;
}

ml_term_t* ml_constructor(token_t operator, ml_term_t* M, ml_term_t* N) {
    char s_operator[2];
    char c_operator;
    if(!operator.is_finite) {
        // Hence, it is an application : M N
        return Applml(M, N);
    }
    switch(operator.val.f_val) {
        case LESS :
            s_operator[0] = '<'; s_operator[1] = '<';
            return Comparison(s_operator, M, N);
        case GREATER :
            s_operator[0] = '>'; s_operator[1] = '>';
            return Comparison(s_operator, M, N);
        case LEQ :
            s_operator[0] = '<'; s_operator[1] = '=';
            return Comparison(s_operator, M, N);
        case GEQ :
            s_operator[0] = '>'; s_operator[1] = '=';
            return Comparison(s_operator, M, N);
        case NEQ :
            s_operator[0] = '<'; s_operator[1] = '>';
            return Comparison(s_operator, M, N);
        case EQUALS :
            s_operator[0] = '='; s_operator[1] = '=';
            return Comparison(s_operator, M, N);
        case PLUS :
            c_operator = '+';
            return ArithForm(c_operator, M, N);
        case MINUS :
            c_operator = '-';
            return ArithForm(c_operator, M, N);
        case TIMES :
            c_operator = '*';
            return ArithForm(c_operator, M, N);
        case DIVIDE :
            c_operator = '/';
            return ArithForm(c_operator, M, N);
        case AND :
            c_operator = '&';
            return BoolForm(c_operator, M, N);
        case OR :
            c_operator = '|';
            return BoolForm(c_operator, M, N);
        case DOT : // FIXME : is this a relevant case ?
        case COMMA :
            return NULL;
        case COLON :
            return NULL;
        case SEMICOLON :
            // The purity of lambda-calculus gives this relation, problem : printing function and so on
            return N;
        case CONCAT_LST :
            return ConcatLst(M, N);
        case CONCAT_STR :
            return ConcatStr(M, N);
        case CONS :
            return List(M, N);
        default : // Hence, it is an application : M N
            return Applml(M, N);
    }
}

void print_ml_term(ml_term_t* term, int indent) {
    for(int i = 0; i < indent; ++i) { fprintf(stderr, " "); }
    switch(term->type) {
        case APPLML :
            fprintf(stderr, "@\n");
            print_ml_term(term->content.appl.applying, indent + 1);
            print_ml_term(term->content.appl.to, indent + 1);
        break;
        case FUNC :
            fprintf(stderr, "fun\n");
            for(int i = 0; i < indent + 1; ++i) { fprintf(stderr, " "); }
            fprintf(stderr, "%s\n", term->content.func.var);
            print_ml_term(term->content.func.body, indent + 1);
        break;
        case DECLARE :
            fprintf(stderr, "let ");
            if(term->content.declare.is_rec) { fprintf(stderr, "rec "); }
            fprintf(stderr, "%s =\n", term->content.declare.var_name);
            print_ml_term(term->content.declare.val, indent + 1);
            fprintf(stderr, " in\n");
            print_ml_term(term->content.declare.in, indent + 1);
        break;
        case COUPLE :
            fprintf(stderr, "(");
            print_ml_term(term->content.func.body, 0);
            fprintf(stderr, ", ");
            print_ml_term(term->content.func.body, 0);
            fprintf(stderr, ")");
        break;
        case LIST :
            // TODO
        break;
        case ARITHM_FORMULA :
            print_ml_term(term->content.a_form.lhs, 0);
            fprintf(stderr, " %c ", term->content.a_form.operator);
            print_ml_term(term->content.a_form.rhs, 0);
        break;
        case BOOL_FORMULA :
            print_ml_term(term->content.b_form.lhs, 0);
            fprintf(stderr, " %c%c ", term->content.b_form.operator, term->content.b_form.operator);
            print_ml_term(term->content.b_form.rhs, 0);
        break;
        case CONDITION :
            free_ml_term(term->content.ite.condition);
            free_ml_term(term->content.ite.body_i);
            free_ml_term(term->content.ite.body_e);
            free(term);
        break;
        case COMPARISON :
            free_ml_term(term->content.compare.lhs);
            free_ml_term(term->content.compare.rhs);
            free(term);
        break;
        case VARIABLE :
            fprintf(stderr,"%s", term->content.var_name);
        break;
        case CONST_INT :
            fprintf(stderr, "%d", term->content.n);
        break;
        case CONST_BOOL :
            char s[5];
            if(term->content.b) {
                s[0] = 't';
                s[1] = 'r';
                s[2] = 'u';
                s[3] = 'e';
                s[4] = 'e';
            } else {
                s[0] = 'f';
                s[1] = 'a';
                s[2] = 'l';
                s[3] = 's';
                s[4] = 'e';
            }
            fprintf(stderr, "%s", s);
        break;
        case CONST_UNIT :
            fprintf(stderr, "()");
        break;
        case CONCAT_LIST :
            // TODO
        break;
        case CONCAT_STRING :
            // TODO
        break;
        case PATTERNMATCH :
            // TODO
        break;
    }
}

void free_ml_term(ml_term_t* term) {
    switch(term->type) {
        case APPLML :
            free_ml_term(term->content.appl.applying);
            free_ml_term(term->content.appl.to);
            free(term);
        break;
        case FUNC :
            free_ml_term(term->content.func.body);
            free(term);
        break;
        case DECLARE :
            free_ml_term(term->content.declare.val);
            free_ml_term(term->content.declare.in);
            free(term);
        break;
        case COUPLE :
            free_ml_term(term->content.cpl.fst);
            free_ml_term(term->content.cpl.snd);
            free(term);
        break;
        case LIST :
            free_ml_term(term->content.lst.hd);
            free_ml_term(term->content.lst.tl);
            free(term);
        break;
        case ARITHM_FORMULA :
            free_ml_term(term->content.a_form.lhs);
            free_ml_term(term->content.a_form.rhs);
            free(term);
        break;
        case BOOL_FORMULA :
            free_ml_term(term->content.b_form.lhs);
            free_ml_term(term->content.b_form.rhs);
            free(term);
        break;
        case CONDITION :
            free_ml_term(term->content.ite.condition);
            free_ml_term(term->content.ite.body_i);
            free_ml_term(term->content.ite.body_e);
            free(term);
        break;
        case COMPARISON :
            free_ml_term(term->content.compare.lhs);
            free_ml_term(term->content.compare.rhs);
            free(term);
        break;
        case VARIABLE :
            free(term);
        break;
        case CONST_INT :
            free(term);
        break;
        case CONST_BOOL :
            free(term);
        break;
        case CONST_UNIT :
            free(term);
        break;
        case CONCAT_LIST :
            free_ml_term(term->content.concat_lst.lst1);
            free_ml_term(term->content.concat_lst.lst2);
            free(term);
        break;
        case CONCAT_STRING :
            free_ml_term(term->content.concat_str.str1);
            free_ml_term(term->content.concat_str.str2);
            free(term);
        break;
        case PATTERNMATCH :
            
        break;
    }
}