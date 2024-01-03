#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ocaml.h"
#include "lexer.h"
#include "parser.h"
#include "utils.h"

void free_lambda_term(lambda_term_t* T) {
    switch(T->type) {
        case APPL :
            free_lambda_term(T->content.appl.applying);
            free_lambda_term(T->content.appl.to);
            free(T);
            break;
        case ABSTR :
            free_lambda_term(T->content.abst.body);
            free(T->content.abst.var_name);
            free(T);
            break;
        case VAR :
            free(T->content.var_name);
            free(T);
            break;
    }
}

void print_lambda_term(lambda_term_t* T, int indent, bool shift) {
    if(shift) { for(int i = 0; i < indent; ++i) { fprintf(stderr, "     "); } }
    switch(T->type) {
        case APPL :
            fprintf(stderr, "@\n");
            print_lambda_term(T->content.appl.applying, indent + 1, true);
            fprintf(stderr, "\n");
            print_lambda_term(T->content.appl.to, indent + 1, true);
            break;
        case ABSTR :
            fprintf(stderr, "^ %s", T->content.abst.var_name);
            int delta = 1;
            lambda_term_t* cur = T->content.abst.body;
            while(cur->type == ABSTR) {
                ++delta;
                fprintf(stderr, ", %s", cur->content.abst.var_name);
                cur = cur->content.abst.body;
            }
            fprintf(stderr, ".");
            print_lambda_term(cur, indent + delta, false);
            break;
        case VAR :
            fprintf(stderr, "%s", T->content.var_name);
            break;
    }
}

// Lambda terms constructors

lambda_term_t* Var(char* var_name) {
    lambda_term_t* res = malloc(sizeof(lambda_term_t));
    res->type = VAR;
    res->content.var_name = malloc(sizeof(char)*(strlen(var_name)+1));
    int i = 0;
    while(var_name[i] != '\0') {
        res->content.var_name[i] = var_name[i];
        ++i;
    }
    res->content.var_name[i] = '\0';
    return res;
}

lambda_term_t* Lambda(char* var_name, lambda_term_t* body) {
    lambda_term_t* res = malloc(sizeof(lambda_term_t));
    res->type = ABSTR;
    res->content.abst.var_name = malloc(sizeof(char)*(strlen(var_name)+1));
    int i = 0;
    while(var_name[i] != '\0') {
        res->content.abst.var_name[i] = var_name[i];
        ++i;
    }
    res->content.abst.var_name[i] = '\0';
    res->content.abst.body = body;
    return res;
}

lambda_term_t* Appl(lambda_term_t* applying, lambda_term_t* to) {
    lambda_term_t* res = malloc(sizeof(lambda_term_t));
    res->type = APPL;
    res->content.appl.applying = applying;
    res->content.appl.to = to;
    return res;
}

// Recursive functions

lambda_term_t* Omega_f(char* f_name) {
    char* var = fresh_var(false);
    return Appl(Lambda(var, Appl(Appl(Var(f_name), Var(var)), Var(var))), Lambda(var, Appl(Appl(Var(f_name), Var(var)), Var(var))));
}

lambda_term_t* Y(char* f_name) {
    return Lambda(f_name, Omega_f(f_name));
}

// Boolean functions

lambda_term_t* lambda_bool(bool b) { // TODO check if it is pertinent to change t and e by fresh variables...
    if(b) {
        return Lambda("t", Lambda("e", Var("t")));
    } else {
        return Lambda("t", Lambda("e", Var("e")));
    }
}

lambda_term_t* Not(void) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    char* var3 = fresh_var(false);
    return Lambda(var1, Lambda(var2, Lambda(var3, Appl(Appl(Var(var1), Var(var3)), Var(var2)))));
}

lambda_term_t* Not_applied(lambda_term_t* b) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    return Lambda(var1, Lambda(var2, Appl(Appl(b, Var(var2)), Var(var1))));
}

// Integer functions

lambda_term_t* f_itere(int k, bool start, char* func_name, char* var_name) {
    // Return the lambda term f^k(x)
    if(k == 0) {
        return Var(func_name);
    } else if(start) {
        return Appl(f_itere(k - 1, false, func_name, var_name), Var(var_name));
    } else {
        return Appl(f_itere(k - 1, false, func_name, var_name), Var(func_name));
    }
}

lambda_term_t* lambda_nat(int n) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    lambda_term_t* res = Lambda(var1, Lambda(var2, f_itere(n, true, var1, var2)));
    free(var1);
    free(var2);
    return res;
}

lambda_term_t* lambda_int(int n) {
    char* tmp = malloc(sizeof(char)*2);
    tmp[0] = 'z';
    tmp[1] = '\0';
    lambda_term_t* res;
    if(n < 0) {
        res = Lambda(tmp, Appl(Appl(Var(tmp), lambda_nat(0)), lambda_nat(n)));
    } else {
        res = Lambda(tmp, Appl(Appl(Var(tmp), lambda_nat(n)), lambda_nat(0)));
    }
    free(tmp);
    return res;
}

lambda_term_t* Eqz(lambda_term_t* n) {
    return Appl(Appl(n, Not()), lambda_bool(true));
}

lambda_term_t* Incr(void) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    char* var3 = fresh_var(false);
    lambda_term_t* res = Lambda(var1, Lambda(var2, Lambda(var3, Appl(Var(var2), Appl(Appl(Var(var1), Var(var2)), Var(var3))))));
    free(var1);
    free(var2);
    free(var3);
    return res;
}

lambda_term_t* Incr_param(char* int_name) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    lambda_term_t* res = Lambda(var1, Lambda(var2, Appl(Var(var1), Appl(Appl(Var(int_name), Var(var1)), Var(var2)))));
    free(var1);
    free(var2);
    return res;
}

lambda_term_t* Decr(void) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    char* var3 = fresh_var(false);
    char* var4 = fresh_var(false);
    char* var5 = fresh_var(false);
    lambda_term_t* res = Lambda(var1, Appl(lambda_bool(true), Appl(Appl(Var(var1),
    Lambda(var2, Appl(Var(var2), Lambda(var3, Lambda(var4, Lambda(var5, Appl(Appl(Var(var5), Incr_param(var4)), Var(var4)))))))),
    Lambda(var5, Appl(Appl(Var(var5), lambda_int(0)), lambda_int(0))))));
    free(var1);
    free(var2);
    free(var3);
    free(var4);
    free(var5);
    return res;
}

lambda_term_t* PlusN(lambda_term_t* n, lambda_term_t* m) {
    return Appl(Appl(n, Incr()), m);
}

lambda_term_t* MinusN(lambda_term_t* n, lambda_term_t* m) {
    return Appl(Appl(m, Decr()), n);
}

lambda_term_t* MultN(lambda_term_t* n, lambda_term_t* m) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    lambda_term_t* res = Lambda(var1, Lambda(var2, Appl(Appl(m, Lambda(var2, PlusN(n, Var(var2)))), lambda_int(0))));
    free(var1);
    free(var2);
    return res;
}

lambda_term_t* DivN(/*lambda_term_t* n, lambda_term_t* m*/void) { // TODO
    return NULL;
}

lambda_term_t* PlusZ(lambda_term_t* n, lambda_term_t* m) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    char* var3 = fresh_var(false);
    lambda_term_t* res = Appl(n, Lambda(var1, Lambda(var2, Lambda(var3, Appl(Appl(Var(var3), PlusN(Var(var1), Appl(lambda_bool(true), m))), PlusN(Var(var2), Appl(lambda_bool(false), m)))))));
    free(var1);
    free(var2);
    free(var3);
    return res;
}

lambda_term_t* MinusZ(lambda_term_t* n, lambda_term_t* m) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    char* var3 = fresh_var(false);
    lambda_term_t* res = PlusZ(n, Appl(m, Lambda(var1, Lambda(var2, Lambda(var3, Appl(Appl(Var(var3), Var(var2)), Var(var1)))))));
    free(var1);
    free(var2);
    free(var3);
    return res;
}

lambda_term_t* MultZ(lambda_term_t* n, lambda_term_t* m) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    char* var3 = fresh_var(false);
    char* var4 = fresh_var(false);
    char* var5 = fresh_var(false);
    lambda_term_t* res =
           Appl(n,
                Appl(m,
                    Lambda(var1, Lambda(var2, Lambda(var3, Lambda(var4, Lambda(var5,
                        Appl(
                            Appl(Var(var5), PlusN(MultN(Var(var1), Var(var3)), MultN(Var(var2), Var(var4))))
                                         , PlusN(MultN(Var(var2), Var(var3)), MultN(Var(var1), Var(var4)))
                            )
                          )))))
                    )
               );
    free(var1);
    free(var2);
    free(var3);
    free(var4);
    free(var5);
    return res;
}

lambda_term_t* DivZ(/*lambda_term_t* n, lambda_term_t* m*/void) { // TODO
    return NULL;
}

lambda_term_t* EqN(lambda_term_t* n, lambda_term_t* m) {
    return Appl(Appl(Eqz(MinusN(n, m)), Eqz(MinusN(m, n))), lambda_bool(false));
}

lambda_term_t* LeqN(lambda_term_t* n, lambda_term_t* m) {
    return Eqz(MinusN(n, m));
}

lambda_term_t* EqZ(lambda_term_t* n, lambda_term_t* m) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    lambda_term_t* res = Appl(n, Lambda(var1, Lambda(var2, Appl(Appl(Eqz(Var(var1)),
                                                EqN(Var(var2), Appl(lambda_bool(false), m))), 
                                                EqN(Var(var1), Appl(lambda_bool(true), m)))
                                     )));
    free(var1);
    free(var2);
    return res;
}

lambda_term_t* LeqZ(lambda_term_t* n, lambda_term_t* m) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    lambda_term_t* res =
    Appl(n, Lambda(var1, Lambda(var2, Appl(Appl(Eqz(Var(var1)),
                                         LeqN(Appl(lambda_bool(true), m), Var(var2))), 
                                         LeqN(Var(var1), Appl(lambda_bool(false), m))))));
    free(var1);
    free(var2);
    return res;
}

lambda_term_t* LtZ(lambda_term_t* n, lambda_term_t* m) {
    char* var1 = fresh_var(false);
    char* var2 = fresh_var(false);
    lambda_term_t* res =
    Appl(n, Lambda(var1, Lambda(var2, Appl(Appl(Eqz(Var(var1)),
                                         LeqN(Appl(Incr(), Appl(lambda_bool(true), m)), Var(var2))), 
                                         LeqN(Incr_param(var1), Appl(lambda_bool(false), m))))));
    free(var1);
    free(var2);
    return res;
}

lambda_term_t* LConcatStr(void) { return NULL; } // TODO

lambda_term_t* LConcatLst(void) { return NULL; } // TODO

// Pattern matching

lambda_term_t* ignore(int n, lambda_term_t* t) { // n the number of arguments, between 0 and 9 to be ignored and t the body of the ignore abstraction (independent from argument)
    if(n == 0) {
        return t;
    } else {
        char* var = fresh_var(false);
        return Lambda(var, ignore(n - 1, t));
    }
}

lambda_term_t* pattern_matching(built_in_pseudo_type_t type, lambda_term_t* t, lambda_term_t* C) { // For a case like : | t -> C
    char* var;
    lambda_term_t* res;
    switch(type) {
        case L_LIST :
            res = Appl(Appl(Appl(Appl(t, ignore(2, lambda_bool(false))), ignore(2, lambda_bool(false))), ignore(2, lambda_bool(false))), C);
            break;
        case L_CPL :
            var = fresh_var(false);
            res = Appl(Appl(Appl(Appl(t, ignore(2, lambda_bool(false))), ignore(2, lambda_bool(false))), Lambda(var, C)), lambda_bool(false));
            free(var);
            break;
        case L_BOOL :
            var = fresh_var(false);
            res = Appl(Appl(Appl(Appl(Appl(Appl(t, ignore(3, lambda_bool(false))), ignore(2, lambda_bool(false))), lambda_bool(false)), Lambda(var, Var(var))), C), lambda_bool(false));
            free(var);
            break;
    }
    return res;
}

lambda_term_t* code_gen(ml_term_t* parsed_code) {
    switch(parsed_code->type) { // TODO implement float
        case APPLML :
            return Appl(code_gen(parsed_code->content.appl.applying), code_gen(parsed_code->content.appl.to));
        case FUNC :
            return Lambda(parsed_code->content.func.var, code_gen(parsed_code->content.func.body));
        case DECLARE :
            if(parsed_code->content.declare.is_rec) {
                return Appl(Lambda(parsed_code->content.declare.var_name, code_gen(parsed_code->content.declare.in)),
                            Appl(Y(parsed_code->content.declare.var_name), Lambda(parsed_code->content.declare.var_name, code_gen(parsed_code->content.declare.val))));
            } else {
                return Appl(Lambda(parsed_code->content.declare.var_name, code_gen(parsed_code->content.declare.in)), code_gen(parsed_code->content.declare.val));
            }
        case VARIABLE :
            return Var(parsed_code->content.var_name);
        case CONST_INT :
            return lambda_int(parsed_code->content.n);
        case CONST_BOOL :
            return lambda_bool(parsed_code->content.b);
        case CONST_UNIT :
            return lambda_bool(false);
        case COUPLE :
            char* var = fresh_var(false);
            lambda_term_t* res = Lambda(var, Appl(Appl(Var(var), code_gen(parsed_code->content.cpl.fst)), code_gen(parsed_code->content.cpl.snd)));
            free(var);
            return res;
        case LIST :
            if(parsed_code->content.lst.hd == NULL) {
                return lambda_bool(false);
            } else {
                char* var1 = fresh_var(false);
                char* var2 = fresh_var(false);
                return Lambda(var1, 
                            Lambda(var2, Appl(Appl(Var(var1), code_gen(parsed_code->content.lst.hd)),
                                             Appl(Appl(code_gen(parsed_code->content.lst.tl), Var(var1)), Var(var2)))));
            }
        case ARITHM_FORMULA : // TODO implement arithmetic operation
            switch(parsed_code->content.a_form.operator) {
                case '+' :
                return PlusZ(code_gen(parsed_code->content.a_form.lhs), code_gen(parsed_code->content.a_form.rhs));
                case '-' :
                return MinusZ(code_gen(parsed_code->content.a_form.lhs), code_gen(parsed_code->content.a_form.rhs));
                case '*' :
                return MultZ(code_gen(parsed_code->content.a_form.lhs), code_gen(parsed_code->content.a_form.rhs));
                case '/' :
                return NULL;
            }
            break;
        case BOOL_FORMULA :
            if(parsed_code->content.b_form.operator == '|') {
                return Appl(Appl(code_gen(parsed_code->content.b_form.lhs), lambda_bool(true)), code_gen(parsed_code->content.b_form.rhs));
            } else {
                return Appl(Appl(code_gen(parsed_code->content.b_form.lhs), code_gen(parsed_code->content.b_form.rhs)), lambda_bool(true));
            }
        case CONDITION :
            return Appl(Appl(code_gen(parsed_code->content.ite.condition), code_gen(parsed_code->content.ite.body_i)), code_gen(parsed_code->content.ite.body_e));
        case COMPARISON :
            if(parsed_code->content.compare.operator[0] == '=' && parsed_code->content.compare.operator[1] == '=') {
                return EqZ(code_gen(parsed_code->content.compare.lhs), code_gen(parsed_code->content.compare.rhs));
            } else if(parsed_code->content.compare.operator[0] == '<' && parsed_code->content.compare.operator[1] == '>') {
                return Not_applied(EqZ(code_gen(parsed_code->content.compare.lhs), code_gen(parsed_code->content.compare.rhs)));
            } else if(parsed_code->content.compare.operator[0] == '<' && parsed_code->content.compare.operator[1] == '=') {
                return LeqZ(code_gen(parsed_code->content.compare.lhs), code_gen(parsed_code->content.compare.rhs));
            } else if(parsed_code->content.compare.operator[0] == '<' && parsed_code->content.compare.operator[1] == '<') {
                return LtZ(code_gen(parsed_code->content.compare.lhs), code_gen(parsed_code->content.compare.rhs));
            } else if(parsed_code->content.compare.operator[0] == '>' && parsed_code->content.compare.operator[1] == '=') {
                return LeqZ(code_gen(parsed_code->content.compare.rhs), code_gen(parsed_code->content.compare.lhs));
            } else if(parsed_code->content.compare.operator[0] == '>' && parsed_code->content.compare.operator[1] == '>') {
                return LtZ(code_gen(parsed_code->content.compare.rhs), code_gen(parsed_code->content.compare.lhs));
            } else {
                fprintf(stderr, "Error : Comparator invalid");
                return NULL;
            }
        case PATTERNMATCH :
            switch(parsed_code->content.match.patterns[0]->type) { // TODO complete
                case LIST :
                    break;
                case COUPLE :
                    break;
                case VARIABLE :
                    break;
                case CONST_INT :
                    break;
                case CONST_BOOL :
                    break;
                case CONST_UNIT :
                    break;
                default :
                    fprintf(stderr, "Error : Pattern invalid");
                    return NULL;
            }
            break;
        case CONCAT_STRING :
            return LConcatStr();
        case CONCAT_LIST :
            return LConcatLst();
    }
    return NULL;
}