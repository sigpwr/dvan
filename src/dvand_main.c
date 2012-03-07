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

dvan_server_t* server = NULL;

void sighandler(int sig){
    printf("signal received\n");
    if (server == NULL)
        fprintf(stderr, "Can't find dvand to terminate\n");
    else
        dvan_server_stop(server);
}

int main(int argc, char ** argv){
    struct sigaction sa;
    int rc = 0;

    server = dvan_server_create(NULL);
    if (server == NULL)
        return -EFAULT;
    
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = 0;
    rc = sigaction(SIGINT, &sa, NULL);
    if (rc != 0){
        fprintf(stderr, "Might not be able to catch ctrl-c\n");
    }

    rc = dvan_server_run(server);
    
    dvan_server_destroy(server);
 
    return rc;
}
