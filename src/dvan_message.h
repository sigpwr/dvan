#ifndef _DVAN_MESSAGE_H
#define _DVAN_MESSAGE_H
#include <stdint.h>
#include "dvan_buffer.h"

struct dvan_message_header {
    uint8_t version;
    uint8_t type;
    uint64_t length;

} __attribute__((packed));

typedef struct dvan_message {
    uint8_t type;
    uint64_t length; 
    void* data;

} dvan_message_t;

dvan_message_t* dvan_message_from_buffer(dvan_buffer_t* b);
dvan_message_t* dvan_message_from_string(char* s);

int dvan_message_to_buffer(dvan_message_t* m, dvan_buffer_t* b);
int dvan_message_to_string(dvan_message_t* m, dvan_buffer_t* b);

#endif
