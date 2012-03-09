#ifndef _DVAN_PARAM_H
#define _DVAN_PARAM_H
#include <stdint.h>
#include "list.h"

enum {
    DVAN_PARAM_STRING,
    DVAN_PARAM_INTEGER,
    DVAN_PARAM_INVALID
};

typedef struct dvan_param_private{
    list_t peers;
    char* key;
    uint8_t type;
    uint32_t length;
    void* value;
} dvan_param_t;

dvan_param_t* dvan_param_create();

dvan_param_t* dvan_param_create_string(char*, char*);
dvan_param_t* dvan_param_create_integer(char*, int);

void dvan_param_dump(dvan_param_t*);
void dvan_param_destroy(dvan_param_t*);

#endif
