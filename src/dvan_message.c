#include <stdlib.h>
#include <string.h>
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

dvan_message_t* dvan_message_create_from_string(char* s){
    dvan_message_t* x;
    if (!s) return NULL;

    x = dvan_message_create();
    if (!x) return NULL;

    x->length = strlen(s) + 1;
    x->data = malloc(x->length);
    if (!x->data){
        dvan_message_destroy(x);
        return NULL;
    }

    strncpy(x->data, s, x->length);
    x->type = DVAN_MESSAGE_STRING;
    return x;
}

int dvan_message_destroy(dvan_message_t* x){
    if (!x) return -EINVAL;

    list_delete(&x->peers);

    if (x->data) free(x->data);
    free(x);

    return 0;
}
