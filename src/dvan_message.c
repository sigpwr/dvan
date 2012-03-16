#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "dvan_param.h"
#include "dvan_message.h"

dvan_message_t* dvan_message_create(){
    dvan_message_t* x;
    x = malloc(sizeof(dvan_message_t));
    if (!x) return NULL;

    x->source_node = NULL;
    x->dest_node = NULL;
    x->error_code = 0;

    list_init(&x->params);

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

int dvan_message_add_string(dvan_message_t* x, char* k, char* v){
    dvan_param_t* dp;
    if (!x || !k || !v) return -EINVAL;
    dp = dvan_param_create_string(k,v);
    if (!dp) return -ENOMEM;
    list_add(&x->params,&dp->peers);
    return 0;
}

int dvan_message_add_integer(dvan_message_t* x, char* k, int v){
    dvan_param_t* dp;
    if (!x || !k || !v) return -EINVAL;
    dp = dvan_param_create_integer(k,v);
    if (!dp) return -ENOMEM;
    list_add(&x->params,&dp->peers);
    return 0;
}

int dvan_message_to_buffer(dvan_message_t* m, dvan_buffer_t* b){
    uint8_t version = 0;
    uint16_t tmp_len, tmp_len_n;
    dvan_param_t* p;
    if (!m || !b) return -EINVAL;
    dvan_buffer_add(b, &version, 1);
    dvan_buffer_add(b, &m->type, 1);

    if (m->source_node)
        tmp_len = strlen(m->source_node);
    else
        tmp_len = 0;
    tmp_len_n = htons(tmp_len);
    dvan_buffer_add(b, &tmp_len_n, sizeof(tmp_len_n));
    dvan_buffer_add(b, m->source_node, tmp_len);

    if (m->dest_node)
        tmp_len = strlen(m->dest_node);
    else
        tmp_len = 0;
    tmp_len_n = htons(tmp_len);
    dvan_buffer_add(b, &tmp_len_n, sizeof(tmp_len_n));
    dvan_buffer_add(b, m->dest_node, tmp_len);
    
    tmp_len = 0;
    list_foreach_entry(p, &m->params, peers){
        tmp_len++;
    } 
    tmp_len_n = htons(tmp_len);
    dvan_buffer_add(b, &tmp_len_n, sizeof(tmp_len_n));

    list_foreach_entry(p, &m->params, peers){
        dvan_param_to_buffer(p, b);
    } 

    return 0;
}

int dvan_message_dump(dvan_message_t* m){
    dvan_param_t* p;
    if (!m) return -EINVAL;
    printf("MESSAGE: type %d\n", m->type);
    list_foreach_entry(p, &m->params, peers){
        dvan_param_dump(p);
    }
    return 0;
}

int dvan_message_to_string(dvan_message_t* m, char* s, uint64_t len){
    if ((!m) || (!s)) return -EINVAL;

//TODO: Fix message_to_string
    return 0;
}

int dvan_message_destroy(dvan_message_t* x){
    if (!x) return -EINVAL;

//TODO: Free data correctly

    list_delete(&x->peers);

    free(x);

    return 0;
}
