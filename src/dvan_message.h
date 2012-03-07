#ifndef _DVAN_MESSAGE_H
#define _DVAN_MESSAGE_H
#include <stdint.h>
#include <time.h>
#include "list.h"
#include "dvan_buffer.h"
#include "dvan_server_client.h"

enum {
    DVAN_MESSAGE_STRING,
    DVAN_MESSAGE_CALL,
    DVAN_MESSAGE_NOTIFICATION
};

struct dvan_message_header {
    uint8_t version;
    uint8_t type;
    uint64_t length;

} __attribute__((packed));

typedef struct dvan_message {
    //Set on creation
    uint8_t type;
    uint64_t length; 
    void* data;

//TODO: Add source and destination
//TODO: Figure out what source and destination types should look like

    //Set by server
    time_t expiry;
    dvan_server_client_t* src;
    list_t peers;
} dvan_message_t;

dvan_message_t* dvan_message_create();
dvan_message_t* dvan_message_from_buffer(dvan_buffer_t* b);
dvan_message_t* dvan_message_from_string(char* s);

int dvan_message_to_buffer(dvan_message_t* m, dvan_buffer_t* b);
int dvan_message_to_string(dvan_message_t* m, char* s, uint64_t len);

int dvan_message_destroy(dvan_message_t* x);

#endif
