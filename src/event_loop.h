#ifndef _EVENT_LOOP_H
#define _EVENT_LOOP_H

#include "list.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

enum el_state {
    EL_RUNNING,
    EL_STOPPED
};

enum el_callbacks {
    EL_READ=0x1,
    EL_WRITE=0x2,
    EL_TIMER=0x4
};

typedef int(*el_callback_t)(int fd, int flags, void* data);

struct el_client {
    struct list l;
    int socket;
    int flags;
    el_callback_t fn;
    void* cbdata;
    int timeout;
} elc_t;

typedef struct el_private {
    int state;
    struct list l;
} el_t;

el_t *el_create();
int el_destroy(el_t *el);
int el_run(el_t *el);
int el_stop(el_t *el);

int el_addfd(el_t *el, int fd);
int el_rmfd(el_t *el, int fd);

int el_setcb(el_t* el, int fd, el_callback_t cbfn, void *cbdata);
int el_setflags(el_t* el, int fd, int flags);
int el_getflags(el_t* el, int fd, int *flags);
int el_settimer(el_t* el, int fd, int seconds);

#endif
