#ifndef _DVAN_SERVER_CLIENT_H
#define _DVAN_SERVER_CLIENT_H

#include <stdint.h>
#include "list.h"
#include "dvan_server.h"
#include "dvan_buffer.h"
#include "utils.h"

enum dvan_server_client_state {
    DC_STATE_NEW
};

typedef struct dvan_server_client_private {
    struct list peers;
    int socket;
    int state;
    uint64_t app_id;
    dvan_buffer_t *in;
    dvan_buffer_t *out;
    dvan_server_t *di;
    struct list messages;
} dvan_server_client_t;

dvan_server_client_t* dvan_server_client_create(dvan_server_t*, int);

int dvan_server_client_setid(dvan_server_client_t*, uint64_t);

int dvan_server_client_destroy(dvan_server_client_t*);

int dvan_server_client_callback(int, int, void*);


#endif
