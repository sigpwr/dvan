#ifndef _DVAN_BUFFER_H
#define _DVAN_BUFFER_H
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#define DVAN_BUFFER_DEFAULT 32

typedef struct dvan_buffer_private {
    char* buffer;
    size_t buf_len;
    size_t data_len;
    size_t data_off;
} dvan_buffer_t;

dvan_buffer_t *dvan_buffer_create();
void dvan_buffer_destroy(dvan_buffer_t* db);

int dvan_buffer_extend(dvan_buffer_t* db);

size_t dvan_buffer_available(dvan_buffer_t* db);

int dvan_buffer_add_uint8(dvan_buffer_t* db, uint8_t x, int version);
int dvan_buffer_add_uint32(dvan_buffer_t* db, uint32_t x, int version);
int dvan_buffer_add_int32(dvan_buffer_t* db, int32_t x, int version);
int dvan_buffer_add_string(dvan_buffer_t* db, char* x, int version);

int dvan_buffer_consume_uint8(dvan_buffer_t* db, uint8_t* dst, int version);
int dvan_buffer_consume_uint32(dvan_buffer_t* db, uint32_t* dst, int version);
int dvan_buffer_consume_int32(dvan_buffer_t* db, int32_t* dst, int version);
int dvan_buffer_consume_string(dvan_buffer_t* db, char** dst, int version);

int dvan_buffer_add(dvan_buffer_t* db, void* src, size_t len);
void* dvan_buffer_consume(dvan_buffer_t* db, size_t len);
void dvan_buffer_consume_reset(dvan_buffer_t* db);
int dvan_buffer_consume_finalize(dvan_buffer_t* db);

int dvan_buffer_isempty(dvan_buffer_t* db);
int dvan_buffer_from_socket(dvan_buffer_t* db, int sk);
int dvan_buffer_to_socket(dvan_buffer_t* db, int sk);

#endif
