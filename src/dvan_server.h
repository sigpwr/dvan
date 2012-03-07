#ifndef _DVAND_H
#define _DVAND_H

#include <signal.h>

#define DVAND_PORT 7589

#include "event_loop.h"

enum dvand_state {
    STATE_RUNNING,
    STATE_STOPPED,
    STATE_STOPPING
};

typedef struct dvand_instance_private {
    int state;
    int server_socket;
    el_t* el;

    struct list clients;
   
    /* Allows us to restore signal handling state on exit */ 
    struct sigaction oldsa;

} dvan_server_t;

dvan_server_t *dvan_server_create(char* config_file);
int dvan_server_run(dvan_server_t*);
int dvan_server_stop(dvan_server_t*);
int dvan_server_destroy(dvan_server_t*);

int dvan_server_socket_create(dvan_server_t*);
int dvan_server_socket_destroy(dvan_server_t*);
int dvan_server_socket_accept(int, void*);

#endif
