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

dvan_message_t* dvan_message_from_string(char* s){
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

dvan_message_t* dvan_message_from_buffer(dvan_buffer_t* b){
    dvan_message_t* x;
    if (!b) return NULL;

    x = dvan_message_create();
    if (!x) return NULL;

//TODO: Add code to parse packet from the buffer here

    x->type = DVAN_MESSAGE_STRING;
    return x;
}

int dvan_message_to_buffer(dvan_message_t* m, dvan_buffer_t* b){

//TODO: Add code to turn message into packet

    return 0;
}

int dvan_message_to_string(dvan_message_t* m, char* s, uint64_t len){
    if ((!m) || (!s)) return -EINVAL;

    if (m->length > len) return -ENOMEM;

    strncpy(s, m->data, m->length);
    return m->length;
}

int dvan_message_destroy(dvan_message_t* x){
    if (!x) return -EINVAL;

    list_delete(&x->peers);

    if (x->data) free(x->data);
    free(x);

    return 0;
}
