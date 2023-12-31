#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ocaml.h"
#include "lexer.h"
#include "parser.h"

void free_lambda_term(lambda_term_t* T) {
    switch(T->type) {
        case APPL :
            free_lambda_term(T->content.appl.applying);
            free_lambda_term(T->content.appl.to);
            free(T);
            break;
        case ABSTR :
            free_lambda_term(T->content.abst.body);
            free(T);
            break;
        case VAR :
            free(T);
            break;
    }
}

void print_lambda_term(lambda_term_t* T, int indent, bool shift) {
    if(shift) { for(int i = 0; i < indent; ++i) { fprintf(stderr, "   "); } }
    switch(T->type) {
        case APPL :
            fprintf(stderr, "@\n");
            print_lambda_term(T->content.appl.applying, indent + 1, true);
            fprintf(stderr, "\n");
            print_lambda_term(T->content.appl.to, indent + 1, true);
            break;
        case ABSTR :
            fprintf(stderr, "^ %s.", T->content.abst.var_name);
            print_lambda_term(T->content.abst.body, indent + 1, false);
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
    res->content.var_name = var_name;
    return res;
}

lambda_term_t* Lambda(char* var_name, lambda_term_t* body) {
    lambda_term_t* res = malloc(sizeof(lambda_term_t));
    res->type = ABSTR;
    res->content.abst.var_name = var_name;
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
    return Appl(Lambda("x", Appl(Appl(Var(f_name), Var("x")), Var("x"))), Lambda("x", Appl(Appl(Var(f_name), Var("x")), Var("x"))));
}

lambda_term_t* Y(char* f_name) {
    return Lambda(f_name, Omega_f(f_name));
}

// Boolean functions

lambda_term_t* lambda_bool(bool b) {
    if(b) {
        return Lambda("t", Lambda("e", Var("t")));
    } else {
        return Lambda("t", Lambda("e", Var("e")));
    }
}

lambda_term_t* Not(void) {
    return Lambda("b", Lambda("t", Lambda("e", Appl(Appl(Var("b"), Var("e")), Var("t")))));
}

lambda_term_t* Not_applied(lambda_term_t* b) {
    return Lambda("t", Lambda("e", Appl(Appl(b, Var("e")), Var("t"))));
}

// Integer functions

lambda_term_t* f_itere(int k, char* func_name, char* var_name) {
    // Return the lambda term f^k(x)
    if(k == 0) {
        return Var(var_name);
    } else {
        return Appl(Var(func_name), f_itere(k - 1, func_name, var_name));
    }
}

lambda_term_t* lambda_int(int n) {
    return Lambda("f", Lambda("x", f_itere(n, "f", "x")));
}

lambda_term_t* Eqz(lambda_term_t* n) {
    return Appl(Appl(n, Not()), lambda_bool(true));
}

lambda_term_t* Incr(void) {
    return Lambda("k", Lambda("f", Lambda("x", Appl(Var("f"), Appl(Appl(Var("k"), Var("f")), Var("x"))))));
}

lambda_term_t* Incr_param(char* int_name) {
    return Lambda("f", Lambda("x", Appl(Var("f"), Appl(Appl(Var(int_name), Var("f")), Var("x")))));
}

lambda_term_t* Decr(void) {
    return Lambda("n", Appl(lambda_bool(true), Appl(Appl(Var("n"),
    Lambda("c", Appl(Var("c"), Lambda("x", Lambda("y", Lambda("z", Appl(Appl(Var("z"), Incr_param("y")), Var("y")))))))),
    Lambda("z", Appl(Appl(Var("z"), lambda_int(0)), lambda_int(0))))));
}

lambda_term_t* Plus(lambda_term_t* n, lambda_term_t* m) {
    return Appl(Appl(n, Incr()), m);
}

lambda_term_t* Minus(lambda_term_t* n, lambda_term_t* m) {
    return Appl(Appl(m, Decr()), n);
}

lambda_term_t* EqN(lambda_term_t* n, lambda_term_t* m) {
    return Appl(Appl(Eqz(Minus(n, m)), Eqz(Minus(m, n))), lambda_bool(false));
}

lambda_term_t* LeqN(lambda_term_t* n, lambda_term_t* m) {
    return Eqz(Minus(n, m));
}

lambda_term_t* EqZ(lambda_term_t* n, lambda_term_t* m) {
    return Appl(n, Lambda("x", Lambda("y", Appl(Appl(Eqz(Var("x")),
                                                EqN(Var("y"), Appl(lambda_bool(false), m))), 
                                                EqN(Var("x"), Appl(lambda_bool(true), m)))
                                     )));
}

lambda_term_t* LeqZ(lambda_term_t* n, lambda_term_t* m) {
    return
    Appl(n, Lambda("x", Lambda("y", Appl(Appl(Eqz(Var("x")),
                                         LeqN(Appl(lambda_bool(true), m), Var("y"))), 
                                         LeqN(Var("x"), Appl(lambda_bool(false), m))))));
}

lambda_term_t* LtZ(lambda_term_t* n, lambda_term_t* m) {
    return
    Appl(n, Lambda("x", Lambda("y", Appl(Appl(Eqz(Var("x")),
                                         LeqN(Appl(Incr(), Appl(lambda_bool(true), m)), Var("y"))), 
                                         LeqN(Incr_param("x"), Appl(lambda_bool(false), m))))));
}

lambda_term_t* LConcatStr(void) { return NULL; } // TODO

lambda_term_t* LConcatLst(void) { return NULL; } // TODO

// Pattern matching

lambda_term_t* ignore(int n, lambda_term_t* t) { // n the number of arguments, between 0 and 9 to be ignored and t the body of the ignore abstraction (independent from argument)
    if(n == 0) {
        return t;
    } else {
        char* var = malloc(sizeof(char));
        var[0] = '0' + n;
        return Lambda(var, ignore(n - 1, t));
    }
}

lambda_term_t* pattern_matching(built_in_pseudo_type_t type, lambda_term_t* t, lambda_term_t* C) {
    switch(type) {
        case L_LIST :
            return Appl(Appl(Appl(Appl(t, ignore(2, lambda_bool(false))), ignore(2, lambda_bool(false))), ignore(2, lambda_bool(false))), C);
        case L_CPL :
            return Appl(Appl(Appl(Appl(t, ignore(2, lambda_bool(false))), ignore(2, lambda_bool(false))), Lambda("x", C)), lambda_bool(false));
        case L_BOOL :
            return Appl(Appl(Appl(Appl(Appl(Appl(t, ignore(3, lambda_bool(false))), ignore(2, lambda_bool(false))), lambda_bool(false)), Lambda("f", Var("f"))), C), lambda_bool("false"));
    }
    return NULL;
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
            if(parsed_code->content.n < 0) {
                return Lambda("z", Appl(Appl(Var("z"), lambda_int(0)), lambda_int(parsed_code->content.n)));
            } else {
                return Lambda("z", Appl(Appl(Var("z"), lambda_int(parsed_code->content.n)), lambda_int(0)));
            }
        case CONST_BOOL :
            if(parsed_code->content.b) {
                return Lambda("t", Lambda("e", Var("t")));
            } else {
                return Lambda("t", Lambda("e", Var("e")));
            }
        case CONST_UNIT :
            return NULL; // TODO
        case COUPLE :
            return Appl(Appl(Var("z"), code_gen(parsed_code->content.cpl.fst)), code_gen(parsed_code->content.cpl.snd));
        case LIST :
            if(parsed_code->content.lst.hd == NULL) {
                return Lambda("c", Lambda("n", Var("n")));
            } else {
                return Lambda("c", 
                            Lambda("n", Appl(Appl(Var("c"), code_gen(parsed_code->content.lst.hd)),
                                             Appl(Appl(code_gen(parsed_code->content.lst.tl), Var("c")), Var("n")))));
            }
        case ARITHM_FORMULA : // TODO implement arithmetic operation
        case BOOL_FORMULA :
            if(parsed_code->content.b_form.operator == '|') {
                return Appl(Appl(code_gen(parsed_code->content.b_form.lhs), Lambda("t", Lambda("e", Var("t")))), code_gen(parsed_code->content.b_form.rhs));
            } else {
                return Appl(Appl(code_gen(parsed_code->content.b_form.lhs), code_gen(parsed_code->content.b_form.rhs)), Lambda("t", Lambda("e", Var("t"))));
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
        case MATCH :
            switch(parsed_code->content.match.patterns[0]->type) { // TODO complete
                case LIST :
                case COUPLE :
                case VARIABLE :
                case CONST_INT :
                case CONST_BOOL :
                case CONST_UNIT :
                default :
                    fprintf(stderr, "Error : Pattern invalid");
                    return NULL;
            }
        case CONCAT_STRING :
            return LConcatStr();
        case CONCAT_LIST :
            return LConcatLst();
    }
    return NULL;
}