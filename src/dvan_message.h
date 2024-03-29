#ifndef _DVAN_MESSAGE_H
#define _DVAN_MESSAGE_H
#include <stdint.h>
#include <time.h>
#include "list.h"
#include "dvan_buffer.h"
#include "dvan_server_client.h"

enum {
    DVAN_MESSAGE_STRING = 1,
    DVAN_MESSAGE_RPC,  
    DVAN_MESSAGE_MULTI,
    DVAN_MESSAGE_INVALID
};

typedef struct dvan_message {
    //Set on creation
    uint8_t type;
    char* source_node;
    char* dest_node;
    int32_t error_code;

    list_t params;

    //Set by server
    time_t expiry;
    dvan_server_client_t* src;
    list_t peers;
} dvan_message_t;

dvan_message_t* dvan_message_create();
dvan_message_t* dvan_message_from_buffer(dvan_buffer_t* b);

int dvan_message_add_string(dvan_message_t*, char* k, char* v);
int dvan_message_add_integer(dvan_message_t*, char* k, int v);

int dvan_message_dump(dvan_message_t* m);
int dvan_message_to_buffer(dvan_message_t* m, dvan_buffer_t* b);

int dvan_message_destroy(dvan_message_t* x);

#endif
