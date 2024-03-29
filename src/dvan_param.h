#ifndef _DVAN_PARAM_H
#define _DVAN_PARAM_H
#include <stdint.h>
#include "list.h"
#include "dvan_buffer.h"

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
dvan_param_t* dvan_param_create_key(char* key);

dvan_param_t* dvan_param_create_string(char*, char*);
dvan_param_t* dvan_param_create_integer(char*, int);
dvan_param_t* dvan_param_from_buffer(dvan_buffer_t*, uint8_t version);

void dvan_param_dump(dvan_param_t*);
int dvan_param_to_buffer(dvan_param_t*, dvan_buffer_t*, uint8_t version);
void dvan_param_destroy(dvan_param_t*);

#endif
