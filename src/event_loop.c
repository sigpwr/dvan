#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "event_loop.h"
#include "utils.h"
#include "list.h"

el_t *el_create(){
    el_t* el;
    el = dvan_malloc(sizeof(el_t));
    el->state = EL_STOPPED;
    list_init(&el->l);
    return el;
}

int el_destroy(el_t *el){
    struct el_client *entry, *tmp;
    list_foreach_entry_safe(entry, tmp, &el->l, l){
        dvan_free(entry);
    }

    dvan_free(el);
    return 0;
}

int el_run(el_t *el){
    struct el_client* tmp, *entry;

    int maxfd, rc;
    time_t start_time, stop_time;
    int flags;

    fd_set read_fds;
    fd_set write_fds;

    struct timespec timeout;
    sigset_t sigs;
    sigemptyset(&sigs);

    if (el->state == EL_RUNNING){
        return -EEXIST;
    }

    el->state = EL_RUNNING;
    while(el->state == EL_RUNNING){
        maxfd = 0;
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        timeout.tv_sec=30;
        timeout.tv_nsec=0;

        list_foreach_entry(tmp, &el->l, l){
           if (tmp->flags & EL_READ){
               FD_SET(tmp->socket, &read_fds);
               maxfd = MAX(maxfd, tmp->socket);
           }
           if (tmp->flags & EL_WRITE){
               FD_SET(tmp->socket, &write_fds);
               maxfd = MAX(maxfd, tmp->socket);
           }
           if (tmp->flags & EL_TIMER){
               timeout.tv_sec = MIN(timeout.tv_sec, tmp->timeout);
           }
        }
        start_time = time(NULL);
        rc = pselect(maxfd+1, &read_fds, &write_fds, NULL, &timeout, &sigs);
        stop_time = time(NULL);
        list_foreach_entry_safe(entry, tmp, &el->l, l){
            flags = 0;
            entry->timeout -= (stop_time - start_time);
            if ((entry->timeout <= 0) && (entry->flags & EL_TIMER)){
                entry->timeout = 0;
                flags |= EL_TIMER;
            }
            if (rc >= 0){
                if (FD_ISSET (entry->socket, &read_fds))
                    flags |= EL_READ;
                if (FD_ISSET (entry->socket, &write_fds))
                    flags |= EL_WRITE;
            }
            if (flags){
                entry->fn(entry->socket, flags, entry->cbdata);
            }
        }
    }
    return 0;
}

int el_stop(el_t *el){
    el->state = EL_STOPPED;
    return 0;
}

int el_addfd(el_t *el, int fd){
    struct el_client* new;

    new = dvan_malloc(sizeof(struct el_client));
    list_init(&new->l);
    new->socket=fd;
    new->flags = 0;
    new->fn = NULL;
    new->cbdata = NULL;
    new->timeout = 0;

    list_add(&el->l, &new->l);
    return 0;
}

int el_rmfd(el_t *el, int fd){
    struct el_client* tmp;

    list_foreach_entry(tmp, &el->l, l){
       if (tmp->socket == fd){
           list_delete(&tmp->l);
           dvan_free(tmp);
           return 0;
       }
    } 
    return -EINVAL;
}

int el_setcb(el_t* el, int fd, el_callback_t cbfn, void* cbdata){
    struct el_client* tmp;

    list_foreach_entry (tmp, &el->l, l){
       if (tmp->socket == fd){
           tmp->fn = cbfn;
           tmp->cbdata = cbdata;
           return 0;
       }
    } 
    return -EINVAL;
}

int el_setflags(el_t *el, int fd, int flags){
    struct el_client* tmp;

    list_foreach_entry(tmp, &el->l, l){
       if (tmp->socket == fd){
           tmp->flags=flags;
           return 0;
       }
    } 
    return -EINVAL;
}

int el_getflags(el_t *el, int fd, int *val){
    struct el_client* tmp;

    list_foreach_entry(tmp, &el->l, l){
       if (tmp->socket == fd){
           *val=tmp->flags;
           return 0;
       }
    } 
    return -EINVAL;
}

int el_settimer(el_t* el, int fd, int seconds){
    struct el_client* tmp;
    if (!el || (seconds < 0))
        return -EINVAL;

    list_foreach_entry(tmp, &el->l, l){
       if (tmp->socket == fd){
           tmp->timeout = seconds;
           return 0;
       }
    } 
    return -EINVAL;
}

