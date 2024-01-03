#include <stdbool.h>

#ifndef OCAML_H
#define OCAML_H

typedef enum token_key {
    IDENTIFIER,
    KEYWORD,
    OPERATOR,
    LITERAL,
    PUNCTUATION
} token_key_t;

typedef enum token_finite_value {
    READING_ERROR,
    PIPE,
    LET,
    REC,
    FUN,
    FUNMATCH,
    MATCH,
    WITH,
    MAPSTO,
    TYPE,
    IN,
    LESS,
    GREATER,
    LEQ,
    GEQ,
    NEQ,
    IF,
    THEN,
    ELSE,
    EQUALS,
    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    CONCAT_LST,
    CONCAT_STR,
    CONS, 
    AND,
    OR,
    NOT,
    QUOTE,
    BEGIN,
    END,
    OPEN_PARENTHESIS,
    END_PARENTHESIS,
    OPEN_SQBRACKET,
    END_SQBRACKET,
    OPEN_BRACKET,
    END_BRACKET,
    OPEN_COMMENT,
    END_COMMENT,
    DOT,
    COMMA,
    COLON,
    SEMICOLON,
    TRUE,
    FALSE
} token_finite_value_t;

typedef union token_value {
    token_finite_value_t f_val; // A predefined value
    char* id_name; // The label of an identifier
    int n; // A literal integer value
} token_value_t;

typedef struct token {
    token_key_t key;
    bool is_finite;
    token_value_t val;
    // Relevant field :
    // .key == IDENTIFIER ---> .id_name
    // .is_finite -----------> .f_val
    // !.is_finite ----------> .n
} token_t;

typedef struct token_list {
    token_t hd;
    struct token_list* tl; 
} token_list_t;

// Syntax

typedef enum ocaml_term_type {
    APPLML,
    FUNC,
    DECLARE,
    VARIABLE,
    CONST_INT,
    CONST_BOOL,
    CONST_UNIT,
    COUPLE,
    LIST,
    CONCAT_LIST,
    CONCAT_STRING,
    ARITHM_FORMULA,
    BOOL_FORMULA,
    CONDITION,
    COMPARISON,
    PATTERNMATCH
} ocaml_term_type_t;

typedef struct ml_term {
    ocaml_term_type_t type;
    union ocaml_content {
        struct declaration {
            char* var_name;
            bool is_rec;
            struct ml_term* val;
            struct ml_term* in;
        } declare;
        struct application_ml {
            struct ml_term* applying;
            struct ml_term* to;
        } appl;
        struct function {
            char* var;
            struct ml_term* body;
        } func;
        struct parenthesis {
            struct ml_term* inside;
        } par;
        struct couple {
            struct ml_term* fst;
            struct ml_term* snd;
        } cpl;
        struct list {
            struct ml_term* hd;
            struct ml_term* tl;
        } lst;
        struct concat_l {
            struct ml_term* lst1;
            struct ml_term* lst2;
        } concat_lst;
        struct concat_s {
            struct ml_term* str1;
            struct ml_term* str2;
        } concat_str;
        struct arithm_formula {
            char operator;
            struct ml_term* lhs;
            struct ml_term* rhs;
        } a_form;
        struct bool_formula {
            char operator;
            struct ml_term* lhs;
            struct ml_term* rhs;
        } b_form;
        struct comparison {
            char operator[2];
            struct ml_term* lhs;
            struct ml_term* rhs;
        } compare;
        struct conditional_branch {
            struct ml_term* condition;
            struct ml_term* body_i;
            struct ml_term* body_e;
        } ite;
        struct pattern_matching {
            struct ml_term* term;
            struct ml_term** patterns;
            struct ml_term** bodys;
        } match;
        char* var_name;
        int n;
        bool b;
    } content;
} ml_term_t;

typedef struct ml_term_lst {
    ml_term_t* hd;
    struct ml_term_lst* tl;
} ml_term_lst_t;

// term lists

void cons(ml_term_t* n_head, ml_term_lst_t** lst);

// Constructors
ml_term_t* Match(ml_term_t* term, ml_term_t** patterns, ml_term_t** bodys);
ml_term_t* Let(ml_term_t* var_name, bool is_rec, ml_term_t* val, ml_term_t* in);
ml_term_t* Applml(ml_term_t* applying, ml_term_t* to);
ml_term_t* Fun(char* var_name, ml_term_t* body);
ml_term_t* Cpl(ml_term_t* fst, ml_term_t* snd);
ml_term_t* List(ml_term_t* hd, ml_term_t* tl);
ml_term_t* ArithForm(char operator, ml_term_t* lhs, ml_term_t* rhs);
ml_term_t* BoolForm(char operator, ml_term_t* lhs, ml_term_t* rhs);
ml_term_t* Comparison(char comparator[2], ml_term_t* lhs, ml_term_t* rhs);
ml_term_t* IfThenElse(ml_term_t* cond, ml_term_t* i, ml_term_t* e);
ml_term_t* ml_var(char* var_name);
ml_term_t* ml_int(int n);
ml_term_t* ml_bool(bool b);
ml_term_t* ml_unit(void);
ml_term_t* ConcatStr(ml_term_t* str1, ml_term_t* str2);
ml_term_t* ConcatLst(ml_term_t* lst1, ml_term_t* lst2);
ml_term_t* ml_constructor(token_t operator, ml_term_t* M, ml_term_t* N);
void print_ml_term(ml_term_t* term, int indent);
void free_ml_term(ml_term_t* term);

#endif // OCAML_H