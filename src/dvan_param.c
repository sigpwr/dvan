#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "dvan_param.h"

dvan_param_t* dvan_param_create(){
    dvan_param_t* x;

    x = malloc(sizeof(dvan_param_t));
    if (!x) return NULL;
    list_init(&x->peers);
    x->type = DVAN_PARAM_INVALID;
    x->length = 0;
    x->value = NULL;

    return x;
}

dvan_param_t* dvan_param_create_key(char* key){
    size_t key_len;
    dvan_param_t* x;

    if (!key) return NULL;

    x = dvan_param_create();
    if (!x) return NULL;

    key_len=strlen(key)+1;
    x->key = malloc(key_len);
    if (!x->key){
        dvan_param_destroy(x);
        return NULL;
    }
    strncpy(x->key, key, key_len);

    return x;
}

dvan_param_t* dvan_param_create_string(char* key, char* value){
    size_t value_len;
    dvan_param_t* x;
    if (!key || !value) return NULL;

    x = dvan_param_create_key(key);
    if (!x) return NULL;

    value_len=strlen(value)+1;

    x->value = malloc(value_len);
    if (!x->value){
        dvan_param_destroy(x);
        return NULL;
    }
    strncpy(x->value, value, value_len);

    x->length = value_len;

    x->type = DVAN_PARAM_STRING;

    return x;
}

dvan_param_t* dvan_param_create_integer(char* key, int value){
    size_t value_len;
    dvan_param_t* x;
    if (!key || !value) return NULL;

    x = dvan_param_create_key(key);
    if (!x) return NULL;

    value_len=sizeof(value);
     
    x->value = malloc(value_len);
    if (!x->value){
        dvan_param_destroy(x);
        return NULL;
    }
    memcpy(x->value, &value, value_len);

    x->length = value_len;
    x->type = DVAN_PARAM_INTEGER;
    return x;
}

dvan_param_t* dvan_param_from_buffer(dvan_buffer_t* b, uint8_t version){
    dvan_param_t* p;
    void* data;
    int rc;
    if (!b) return NULL;
    p = dvan_param_create();
    rc = dvan_buffer_consume_string(b, &p->key, version);
    if (rc) goto dvan_param_from_buffer_out;
    rc = dvan_buffer_consume_uint8(b, &p->type, version);
    if (rc) goto dvan_param_from_buffer_out;
    rc = dvan_buffer_consume_uint32(b, &p->length, version);
    if (rc) goto dvan_param_from_buffer_out;

    data = dvan_buffer_consume(b, p->length);
    if (!data) goto dvan_param_from_buffer_out;

    p->value = malloc(p->length);
    if (!p->value) goto dvan_param_from_buffer_out;
    memcpy(p->value, data, p->length);

    return p;

dvan_param_from_buffer_out:
    dvan_buffer_consume_reset(b);
    dvan_param_destroy(p);
    return NULL;
}

int dvan_param_to_buffer(dvan_param_t* p, dvan_buffer_t* b, uint8_t version){
    int rc;
    if (!p || !b) return -EINVAL;
    rc = dvan_buffer_add_string(b, p->key, version);
    if (rc) goto dvan_param_to_buffer_out;
    rc = dvan_buffer_add_uint8(b, p->type, version);
    if (rc) goto dvan_param_to_buffer_out;
    rc = dvan_buffer_add_uint32(b, p->length, version);
    if (rc) goto dvan_param_to_buffer_out;
    rc = dvan_buffer_add(b, p->value, p->length);
    if (rc) goto dvan_param_to_buffer_out;
    
    return 0;

dvan_param_to_buffer_out:
    dvan_param_destroy(p);
    return rc;
}

void dvan_param_dump(dvan_param_t* x){
    if (!x) return;

    switch (x->type){
        case DVAN_PARAM_STRING:
            printf("\t%s - (STRING)%s\n", x->key, (char*)x->value);
            break;
        case DVAN_PARAM_INTEGER:
            printf("\t%s - (INTEGER)%d\n", x->key, *((int*)x->value));
            break;
        default:
            printf("\t%s - (UNKNOWN)%p\n", x->key, x->value);
    }
    return;
}

void dvan_param_destroy(dvan_param_t* x){
    if (!x) return;

    list_delete(&x->peers);
    if (x->key) free(x->key);
    if (x->value) free(x->value);
    free(x);
    return;
}


