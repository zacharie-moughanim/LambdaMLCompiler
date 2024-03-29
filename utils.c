#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"

// linked lists of string
/*
void cons(char* n_head, str_lst_t** lst) {
    str_lst_t* n_lst = malloc(sizeof(str_lst_t));
    int len = strlen(n_head);
    n_lst->hd = malloc(sizeof(char)*len);
    strcpy(n_head, n_lst->hd);
    n_lst->tl = (*lst);
    (*lst) = n_lst;
}*/

char* fresh_var(bool end) {
    static int len = 1;
    static bool full = true;
    static bool allocated = false;
    static char* fresh;
    char* res = NULL;
    if(!allocated) {
        fresh = malloc(sizeof(char));
        allocated = true;
    }
    if(end) {
        free(fresh);
    } else {
        if(full) {
            ++len;
            fresh = (char*) realloc(fresh, sizeof(char)*len);
            fresh[0] = '$';
            for(int i = 1; i < len; ++i) {
                fresh[i] = 'a';
            }
            full = false;
        } else {
            int i = len - 1;
            while(fresh[i] == 'z') {
                fresh[i] = 'a';
                --i;
            }
            ++fresh[i];
            if(fresh[i] == 'z' && i == 1) { full = true; }
        }
        res = malloc(sizeof(char)*(len+1));
        for(int i = 0; i < len; ++i) {
            res[i] = fresh[i];
        }
        res[len] = '\0';
    }
    return res;
}