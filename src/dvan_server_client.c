#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "dvan_server_client.h"
#include "dvan_message.h"
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
    list_init(&client->messages);
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
    dvan_message_t *x, *t;
    printf("Deleting client\n");
    el_rmfd(tmp->di->el, tmp->socket);
    list_delete(&tmp->peers);
 
    list_foreach_entry_safe(x, t, &tmp->messages, peers){
        dvan_message_destroy(x);
    }

    dvan_buffer_destroy(tmp->in);
    dvan_buffer_destroy(tmp->out);
    dvan_free(tmp);
    return 0;
}

int dvan_server_client_callback(int fd, int flags, void* cbd){
    int bytes;
    int new_flags;
    dvan_server_client_t* client;
    dvan_message_t* m;
    client=(dvan_server_client_t*)cbd;

    if (flags & EL_READ){
        bytes = dvan_buffer_from_socket(client->in, fd);
        if (bytes <= 0){
            dvan_server_client_delete(client);
            return 0;
        }
        m = dvan_message_from_buffer(client->in);
        if (m){
            list_add(&client->messages, &m->peers);
            printf("We've got a message!\n");
        }
    } else if (flags & EL_WRITE) {
        bytes = dvan_buffer_to_socket(client->out, fd);
        if (bytes <= 0){
            dvan_server_client_delete(client);
            return 0;
        }
    }

#ifdef DVAN_DEBUG
    list_foreach_entry(m, &client->messages, peers){
        dvan_message_dump(m);
    }
#endif

    new_flags = EL_READ;
    if (!dvan_buffer_isempty(client->out))
        new_flags |= EL_WRITE;
    
    el_setflags(client->di->el, fd, new_flags);
    return 0;
}

