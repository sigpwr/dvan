#ifndef _DVAN_BUFFER_H
#define _DVAN_BUFFER_H
#include <unistd.h>
#include <errno.h>

#define DVAN_BUFFER_DEFAULT 32

typedef struct dvan_buffer_private {
    char* buffer;
    size_t buf_len;
    size_t data_len;
} dvan_buffer_t;

dvan_buffer_t *dvan_buffer_create();
void dvan_buffer_destroy(dvan_buffer_t* db);

int dvan_buffer_extend(dvan_buffer_t* db);

size_t dvan_buffer_available(dvan_buffer_t* db);
size_t dvan_buffer_size(dvan_buffer_t* db);
size_t dvan_buffer_copy(dvan_buffer_t* db, void* targ, size_t len);
size_t dvan_buffer_consume(dvan_buffer_t* db, size_t len);

int dvan_buffer_add(dvan_buffer_t* db, void* src, size_t len);

int dvan_buffer_isempty(dvan_buffer_t* db);
int dvan_buffer_from_socket(dvan_buffer_t* db, int sk);
int dvan_buffer_to_socket(dvan_buffer_t* db, int sk);

#endif
