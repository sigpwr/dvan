#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "dvan_param.h"

dvan_param_t* dvan_param_create(char* key){
    size_t key_len;
    dvan_param_t* x;

    if (!key) return NULL;

    x = malloc(sizeof(dvan_param_t));
    if (!x) return NULL;
    list_init(&x->peers);
    x->type = DVAN_PARAM_INVALID;
    x->length = 0;
    x->value = NULL;

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

    x = dvan_param_create(key);
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

    x = dvan_param_create(key);
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


