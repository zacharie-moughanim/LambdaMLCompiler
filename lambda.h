#ifndef LAMBDA_H
#define LAMBDA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef enum built_in_pseudo_type {
    L_LIST,
    L_CPL,
    L_BOOL
} built_in_pseudo_type_t;


typedef enum type {
    APPL,
    ABSTR,
    VAR
} type_lambda_t;

typedef struct lambda {
    type_lambda_t type;
    union lambda_content {
        // Variable
        char* var_name;

        // Application
        struct application_l {
            struct lambda* applying;
            struct lambda* to;
        } appl;

        // Abstraction
        struct abstraction {
            char* var_name;
            struct lambda* body;
        } abst;
    } content;
} lambda_term_t;


typedef enum type_inter {
    IT_APPL,
    IT_ABSTR,
    IT_VAR,
    IT_DECL
} type_inter_t;

typedef struct inter_term {
    type_inter_t type;
    union inter_content {
        // Variable
        char* var_name;

        // Application

        struct it_application {
            struct inter_term* applying;
            struct inter_term* to;
        } it_appl;

        // Abstraction
        struct inter_abstraction {
            char* var_name;
            struct inter_term* body;
        } it_abst;

        // Declaration

        struct declare {
            char* var_name;
            struct inter_term* val;
            struct inter_term* in;
        } it_decl;
    } it_content;
} inter_t;

#endif // LAMBDA_H