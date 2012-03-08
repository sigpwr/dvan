#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include "dvan_server.h"
#include "dvan_server_client.h"
#include "utils.h"
#include "list.h"

int dvan_server_socket_callback(int sk, int flags, void* cb){
    static uint64_t last_client = 0;
    dvan_server_t* di;
    dvan_server_client_t* dc;
    int client_sk;
    struct sockaddr_in6 sa;
    socklen_t sl;

    di = (dvan_server_t*)cb;

    if (flags & EL_READ){
        sl = sizeof(sa);
        client_sk = accept(sk, (struct sockaddr*)&sa, &sl); 
        printf("accepted socket %d\n", client_sk);
        dc = dvan_server_client_create(di, client_sk);
        if (dc){
            dvan_server_client_setid(dc, last_client++);
        }
    } else if (flags & EL_TIMER) {
        printf("DVAN Server alive\n");
        el_settimer(di->el, sk, 10);
    }
    return 0;
}

dvan_server_t *dvan_server_create(char* config_file){
    dvan_server_t *rv;
    rv = dvan_malloc(sizeof(dvan_server_t));
    rv->state = STATE_STOPPED;

    rv->el = el_create();
    if (!rv->el)
        goto dvan_server_create_no_el;
    list_init(&rv->clients);
    
    if (dvan_server_socket_create(rv))
        goto dvan_server_create_no_ss;

    return rv;

dvan_server_create_no_ss:
    el_destroy(rv->el);
dvan_server_create_no_el:
    dvan_free(rv);
    return NULL;
}

int dvan_server_destroy(dvan_server_t* di){
    int rc;
    dvan_server_client_t *x, *y;
    if (di->state == STATE_RUNNING){
        fprintf(stderr, "dvan_server stopping\n");
        dvan_server_stop(di);
    }
    if (di->state != STATE_STOPPED){
        fprintf(stderr, "Error, dvan_server not stopped\n");
        return -EAGAIN;
    }

    dvan_server_socket_destroy(di);

    list_foreach_entry_safe(x, y, &di->clients, peers){
        dvan_server_client_destroy(x);
    }

    if (di->el){
        rc = el_destroy(di->el);
        if (rc == 0)
            di->el = NULL;
        else
            return rc;
    }

    
    dvan_free(di);
    return 0;
}

int dvan_server_run(dvan_server_t* di){
    int rc;
    di->state = STATE_RUNNING;
    assert(di->el);
    rc = el_run(di->el);
    return rc;
}

int dvan_server_stop(dvan_server_t* di){
    int rc;
    rc = el_stop(di->el);
    if (!rc)
        di->state = STATE_STOPPED;

    return 0;
}

int dvan_server_socket_create(dvan_server_t* di){
    struct sockaddr_in6 sa;
    int sock, rc, opt;
    socklen_t sl;
    sock = socket(AF_INET6, SOCK_STREAM, 0); 
    if (sock < 0){
        return sock;
    }
   
    opt=1;
    rc = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (rc < 0){
        close(sock);
        return rc;
    }
  
   
    memset(&sa, 0, sizeof(struct sockaddr_in6));
    sa.sin6_family = AF_INET6;
    sa.sin6_port = htons(DVAND_PORT);
    memcpy(&sa.sin6_addr, &in6addr_any, sizeof(struct in6_addr));

    rc = bind(sock, (struct sockaddr*)&sa, sizeof(sa));
    if (rc < 0){
        close(sock);
        return rc;
    }

    rc = fcntl(sock, F_SETFL, O_NONBLOCK);
    if (rc < 0){
        printf("nonblock failed\n");
        close(sock);
        return rc;
    }

    sl = sizeof(struct sockaddr_in6);
    rc = getsockname(sock, (struct sockaddr*)&sa, &sl);
    if (rc < 0){
        printf("getsockname failed\n");
        close(sock);
        return rc;
    }

    rc = listen(sock, 0);
    if (rc < 0){
        printf("listen failed\n");
        close(sock);
        return rc;
    }

    printf("dvan_server listening on port %d\n", ntohs(sa.sin6_port));

    el_addfd(di->el, sock);
    el_setcb(di->el, sock, dvan_server_socket_callback, (void*) di);
    el_settimer(di->el, sock, 10);
    el_setflags(di->el, sock, EL_READ | EL_TIMER);

    di->server_socket = sock;
    return 0;
}

int dvan_server_socket_destroy(dvan_server_t* di){
    el_rmfd(di->el, di->server_socket);
    close(di->server_socket);
    return 0;
}
