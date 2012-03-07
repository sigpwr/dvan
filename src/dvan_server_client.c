#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "dvan_server_client.h"
#include "dvan_server.h"

int dvan_server_client_add(dvan_server_t* di, int fd){
    int rc;
    dvan_server_client_t* client;

    client=dvan_malloc(sizeof(dvan_server_client_t));
    client->state=DC_STATE_NEW;
    client->socket=fd;
    client->di=di;
    client->in = dvan_buffer_create();
    client->out = dvan_buffer_create();
    list_init(&client->peers);
    list_add(&di->clients, &client->peers);
    rc=el_addfd(di->el, fd);
    if (rc){
        dvan_free(client);
        return rc;
    }
    rc = el_setcb(di->el, fd, dvan_server_client_callback, client);
    if (rc){
        el_rmfd(di->el, fd);
        dvan_free(client);
        return rc;
    }
    rc=el_setflags(di->el, fd, EL_READ);

    return 0;
}

int dvan_server_client_delete(dvan_server_client_t *tmp){
    printf("Deleting client\n");
    el_rmfd(tmp->di->el, tmp->socket);
    list_delete(&tmp->peers);
    dvan_buffer_destroy(tmp->in);
    dvan_buffer_destroy(tmp->out);
    dvan_free(tmp);
    return 0;
}

int dvan_server_client_callback(int fd, int flags, void* cbd){
    int bytes;
    int new_flags;
    dvan_server_client_t* client;
    client=(dvan_server_client_t*)cbd;

    if (flags & EL_READ){
        bytes = dvan_buffer_from_socket(client->in, fd);
        if (bytes <= 0){
            dvan_server_client_delete(client);
            return 0;
        }
        fprintf(stderr, "%s", client->in->buffer); 
    } else if (flags & EL_WRITE) {
        bytes = dvan_buffer_to_socket(client->out, fd);
        if (bytes <= 0){
            dvan_server_client_delete(client);
            return 0;
        }
    }
    new_flags = EL_READ;
    if (!dvan_buffer_isempty(client->out))
        new_flags |= EL_WRITE;
    
    el_setflags(client->di->el, fd, new_flags);
    return 0;
}

