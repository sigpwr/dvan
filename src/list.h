#ifndef _LIST_H
#define _LIST_H

typedef struct list {
    struct list *next;
    struct list *prev;
} list_t;

#define list_get(list, type, field) ((type *)((char *)(list) - (unsigned long long)(&((type *)0)->field)))

#define list_get_first(list, type, field) ((type *)((char *)((list)->next) - (unsigned long long)(&((type *)0)->field)))

#define list_foreach(cur, list) \
    for (cur = (list)->next; cur != (list); cur=cur->next)

#define list_foreach_entry(current, list, field) \
    for (current = list_get((list)->next, typeof(*current), field); \
    &current->field != (list); \
    current = list_get(current->field.next, typeof(*current), field))

#define list_foreach_entry_safe(current, tmp, list, field)          \
    for (current=list_get((list)->next, typeof(*current), field),   \
        tmp = list_get(current->field.next, typeof(*current), field); \
        &current->field != (list);                                  \
        current=tmp, tmp=list_get(tmp->field.next, typeof(*tmp), field))

void list_init(struct list* x);
void list_add(struct list* hdr, struct list* x);
void list_delete(struct list* x);
int list_isempty(struct list* x);

#endif
