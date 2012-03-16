#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "dvan_buffer.h"
#include "dvan_message.h"

#define DVAND_PORT 7589

int main(int argc, char** argv){
    int sk = -1;
    int rc;
    int i;
    struct addrinfo hints, *res, *res0;
    dvan_buffer_t* out;
    dvan_message_t* msg;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    rc = getaddrinfo("localhost", "7589", &hints, &res0);
    if (rc){
        printf("getaddrinfo fail\n");
        close(sk);
        return rc;
    }

    for (res = res0; res; res=res->ai_next){
        sk = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sk < 0){
            printf("socket fail\n");
            continue;
        }

        if (connect(sk, res->ai_addr, res->ai_addrlen) < 0){
            printf("connect failed\n");
            close(sk);
            continue;
        }
        break;

    }
    if (sk < 0){
        printf("socket complete fail\n");
        return 0;
    }

    out = dvan_buffer_create();
    if (!out) return 0;

    msg = dvan_message_create();
    if (!msg){
        printf("message fail\n");
        return 0;
    }

    for (i = 1; i < argc; i+=2){
        if (i+1 < argc){
            dvan_message_add_string(msg, argv[i], argv[i+1]);
        }
    }
    printf("Have message %p\n", msg);
    dvan_message_dump(msg);
    rc = dvan_message_to_buffer(msg, out); 
    if (rc) printf("Message to buffer fail\n");
    rc = dvan_buffer_to_socket(out, sk); 
    if (rc<0) printf("buffer to socket fail (%d)\n", rc);

    close(sk);

    printf("hi\n");
    return 0;
}
