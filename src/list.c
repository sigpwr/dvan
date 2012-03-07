#include <stdlib.h>
#include "list.h"

void list_init(struct list* x){
    x->next = x;
    x->prev = x;
}

void list_add(struct list* hdr, struct list* x){
    x->next = hdr->next;
    x->prev = hdr;
    x->next->prev = x;
    x->prev->next = x;
}

void list_delete(struct list* x){
    x->next->prev = x->prev;
    x->prev->next = x->next;
    x->next = NULL;
    x->prev = NULL;
}

int list_isempty(struct list* x){
    return x->next == x;
}

