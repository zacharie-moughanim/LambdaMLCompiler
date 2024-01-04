#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lambda.h"
#include "lambda-calculus_interpreter.h"

lambda_term_t* substitution(lambda_term_t* M, char* var, lambda_term_t* N) { // Return M[x := N]
    lambda_term_t* res = NULL;
    switch(M->type) {
        case VAR :
            if(strcmp(M->content.var_name, var) == 0) {
                free_lambda_term(M);
                res = lambda_copy(N);
            } else {
                res = M;
            }
        break;
        case ABSTR :
            if(strcmp(M->content.abst.var_name, var) == 0) {
                fprintf(stderr, "Error : check interpreter");
                res = NULL;
            } else {
                res = Lambda(M->content.abst.var_name, substitution(M->content.abst.body, var, N));
                free(M->content.abst.var_name);
                free(M);
            }
        break;
        case APPL :
            res = Appl(substitution(M->content.appl.applying, var, N), substitution(M->content.appl.to, var, N));
            free(M);
        break;
    }
    return res;
}

lambda_term_t* evaluation_call_by_name(lambda_term_t* T, bool* is_modified) {
    lambda_term_t* res;
    switch(T->type) {
        case VAR :
            res = T;
        break;
        case ABSTR :
            res = T;
        break;
        case APPL :
            *is_modified = true;
            lambda_term_t* M = T->content.appl.applying;
            lambda_term_t* N = T->content.appl.to;
            lambda_term_t* M_ev = lambda_interpreter(M);
            if(M_ev->type != ABSTR) {
                lambda_term_t* N_ev = lambda_interpreter(N);
                res = Appl(M_ev, N_ev);
            } else {
                res = lambda_interpreter(substitution(M_ev->content.abst.body, M_ev->content.abst.var_name, N));
                free_lambda_term(N);
                free(M_ev->content.abst.var_name);
                free(M_ev);
            }
            free(T);
        break;
    }
    return res;
}

lambda_term_t* lambda_interpreter(lambda_term_t* T) {
    lambda_term_t* res = T;
    bool was_modified;
    while(true) {
        was_modified = false;
        res = evaluation_call_by_name(res, &was_modified);
        if(!was_modified) {
            if(res->type == ABSTR) {
                fprintf(stderr,"A\n");
                lambda_term_t* tmp = res;
                res = Lambda(tmp->content.abst.var_name, lambda_interpreter(tmp->content.abst.body));
                free(tmp->content.abst.var_name);
                free(tmp);
            }
            return res;
        }
    }
}