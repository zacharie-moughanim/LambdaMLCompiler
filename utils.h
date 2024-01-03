#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

typedef struct str_lst {
    char* hd;
    struct str_lst* tl;
} str_lst_t;

str_lst_t* str_lst_init(void);
str_lst_t* str_lst_init(void);

// In Caml, no variable can start with an '$', so this function return a fresh variable starting by a '$'
// after every call to [fresh_var] are done, you shall call [fresh_var(true)].
// WARNING : once this call is maed, no further call can be done.
char* fresh_var(bool end);
void cons_str_lst(char* n_head, str_lst_t** lst);

#endif // UTILS_H