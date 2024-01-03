#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lambda.h"
#include "lambda-calculus_interpreter.h"

lambda_term_t* lambda_copy(lambda_term_t* M) {
    lambda_term_t* res = malloc(sizeof(lambda_term_t));
    switch(M->type) {
        case VAR :
            res = Var(M->content.var_name);
        break;
        case ABSTR :
            res = Lambda(M->content.abst.var_name, lambda_copy(M->content.abst.body));
        break;
        case APPL :
            res = Appl(lambda_copy(M->content.appl.applying), lambda_copy(M->content.appl.to));
        break;
    }
    return res;
}

lambda_term_t* substitution(lambda_term_t* M, char* var, lambda_term_t* N) { // Return M[x := N]
    lambda_term_t* res = NULL;
    switch(M->type) {
        case VAR :
            if(strcmp(M->content.var_name, var) == 0) {
                res = lambda_copy(N);
            } else {
                res = M;
            }
        break;
        case ABSTR :
            res = Lambda(M->content.abst.var_name, substitution(M->content.abst.body, var, N));
        break;
        case APPL :
            res = Appl(substitution(M->content.appl.applying, var, N), substitution(M->content.appl.to, var, N));
        break;
    }
    return res;
}

lambda_term_t* lambda_interpreter(lambda_term_t* T) {
    switch(T->type) {
        case VAR :
            return T;
        break;
        case ABSTR :
            return T;
        break;
        case APPL :
            lambda_term_t* M = T->content.appl.applying;
            lambda_term_t* N = T->content.appl.to;
            lambda_term_t* M_ev = lambda_interpreter(M);
            if(M_ev->type != ABSTR) {
                lambda_term_t* N_ev = lambda_interpreter(N);
                return Appl(M_ev, N_ev);
            } else {
                return lambda_interpreter(substitution(M_ev->content.abst.body, M_ev->content.abst.var_name, N));
            }
        break;
    }
    return NULL;
}