#include <stdlib.h>
#include "dvan_message.h"

dvan_message_t* dvan_message_create(){
    dvan_message_t* x;
    x = malloc(sizeof(dvan_message_t));
    if (!x) return NULL;

    x->type = 0;
    x->length = 0;
    x->data = NULL;

    x->expiry = 0;
    x->src = NULL;
    list_init(&x->peers);
    return x;
}


