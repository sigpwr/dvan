#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
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
    uint8_t version;
    uint32_t num_params;
    uint32_t i;
    int rc;
    dvan_message_t* x;
    dvan_param_t* p;

    if (!b) return NULL;

    x = dvan_message_create();
    if (!x) return NULL;
    rc = dvan_buffer_consume_uint8(b, &version, 0); 
    if (rc) goto dvan_message_from_buffer_out;
    rc = dvan_buffer_consume_uint8(b, &x->type, version); 
    if (rc) goto dvan_message_from_buffer_out;
    rc = dvan_buffer_consume_string(b, &x->source_node, version); 
    if (rc) goto dvan_message_from_buffer_out;
    rc = dvan_buffer_consume_string(b, &x->dest_node, version); 
    if (rc) goto dvan_message_from_buffer_out;
    rc = dvan_buffer_consume_int32(b, &x->error_code, version); 
    if (rc) goto dvan_message_from_buffer_out;
    rc = dvan_buffer_consume_uint32(b, &num_params, version); 
    if (rc) goto dvan_message_from_buffer_out;
    /* Source node name */

    for (i=0; i<num_params; i++){
        p = dvan_param_from_buffer(b, version); 
        if (!p) goto dvan_message_from_buffer_out;
        list_add(&x->params, &p->peers);
    }
    return x;

dvan_message_from_buffer_out:
    dvan_message_destroy(x);
    dvan_buffer_consume_reset(b);
    return NULL;
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
    uint32_t tmp_len = 0;
    dvan_param_t* p;
    if (!m || !b) return -EINVAL;
    dvan_buffer_add_uint8(b, version, version);
    dvan_buffer_add_uint8(b, m->type, version);
    dvan_buffer_add_string(b, m->source_node, version);
    dvan_buffer_add_string(b, m->dest_node, version);
    dvan_buffer_add_int32(b, m->error_code, version);
    list_foreach_entry(p, &m->params, peers){
        tmp_len++;
    } 
    dvan_buffer_add_uint32(b, tmp_len, version); 
    list_foreach_entry(p, &m->params, peers){
        dvan_param_to_buffer(p, b, version);
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
