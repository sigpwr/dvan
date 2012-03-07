#ifndef _DVAN_SERVER_CLIENT_H
#define _DVAN_SERVER_CLIENT_H

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
    dvan_buffer_t *in;
    dvan_buffer_t *out;
    dvan_server_t *di;
} dvan_server_client_t;

int dvan_server_client_add(dvan_server_t*, int);
int dvan_server_client_delete(dvan_server_client_t*);

int dvan_server_client_callback(int, int, void*);


#endif
