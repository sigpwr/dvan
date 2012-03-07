#include <stdlib.h>
#include <stdio.h>
#include "list.h"

struct lt{
    struct list l;
    int x;
};

int main(int argc, char** argv){
    struct list* head = NULL;
    struct lt* tmp=NULL;
    struct list* ltmp;


    head = malloc(sizeof(struct list));
    list_init(head);

    tmp=malloc(sizeof(struct lt));
    list_init(&tmp->l);
    tmp->x = 2;
    list_add(head, &tmp->l);
    
    tmp=malloc(sizeof(struct lt));
    list_init(&tmp->l);
    tmp->x = 3;
    list_add(head, &tmp->l);

    tmp=malloc(sizeof(struct lt));
    list_init(&tmp->l);
    tmp->x = 4;
    list_add(head, &tmp->l);

    tmp=malloc(sizeof(struct lt));
    list_init(&tmp->l);
    tmp->x = 5;
    list_add(head, &tmp->l);

    list_foreach(ltmp, head){
       tmp=list_get(ltmp, struct lt, l);
       printf("x is %d\n", tmp->x); 
    }

    printf("hi\n");
    printf("%p\n", head);
    return 0;
}
