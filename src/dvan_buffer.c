#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "dvan_buffer.h"
#include "utils.h"

dvan_buffer_t *dvan_buffer_create(){
    dvan_buffer_t *b;
    b = (dvan_buffer_t*) dvan_malloc(sizeof(struct dvan_buffer_private));
    if (!b) goto dvan_malloc_out;

    b->buffer = dvan_malloc(DVAN_BUFFER_DEFAULT);   
    if (!b->buffer) goto dvan_buffer_malloc_out; 

    b->buf_len = DVAN_BUFFER_DEFAULT;
    b->data_len = 0;
    b->data_off = 0;
    return b;

dvan_buffer_malloc_out:
    dvan_free(b);
dvan_malloc_out:
    return NULL;
}


void dvan_buffer_destroy(dvan_buffer_t* db){
    if (!db) return;

    if (db->buffer) dvan_free(db->buffer);

    dvan_free(db);

    return;
}

int dvan_buffer_extend(dvan_buffer_t* db){
    void* tmp;
    if (!db) return -EINVAL;
    
    tmp = dvan_realloc(db->buffer, db->buf_len * 2);

    if (tmp){
        db->buffer = tmp;
        db->buf_len = db->buf_len * 2;
    } 

    return 0;
}

size_t dvan_buffer_available(dvan_buffer_t* db) { 
    return (db)?(db->buf_len - db->data_len):-EINVAL; 
}

int dvan_buffer_add_uint8(dvan_buffer_t* db, uint8_t x, int version){
    return dvan_buffer_add(db, &x, sizeof(x));
}
int dvan_buffer_add_uint32(dvan_buffer_t* db, uint32_t x, int version){
    x = htonl(x);
    return dvan_buffer_add(db, &x, sizeof(x));
}
int dvan_buffer_add_int32(dvan_buffer_t* db, int32_t x, int version){
    x = htonl(x);
    return dvan_buffer_add(db, &x, sizeof(x));
}
int dvan_buffer_add_string(dvan_buffer_t* db, char* x, int version){
    int rc;
    uint32_t s;
    if (!x) {
        s=0;
    } else {
        s=strlen(x);
    }
    rc = dvan_buffer_add_uint32(db, s, version);
    if (rc) return rc;
    return dvan_buffer_add(db, x, s);
}

int dvan_buffer_add(dvan_buffer_t* db, void* src, size_t len){
    int rc;

    if (len == 0)
        return 0;
    if (!db || !src)
        return -EINVAL;

    while (len > dvan_buffer_available(db)){
        rc = dvan_buffer_extend(db); 
        if (rc != 0)
            return rc;
    }
    
    memcpy(db->buffer + db->data_len, src, len); 
    db->data_len += len;
    return rc;
}

int dvan_buffer_consume_uint8(dvan_buffer_t* db, uint8_t* dst, int version){
    void* tmp;
    tmp = dvan_buffer_consume(db, sizeof(*dst));
    if (!tmp) return -ENOMEM;
    *dst = *((uint8_t*)tmp);
    return 0;
}

int dvan_buffer_consume_uint32(dvan_buffer_t* db, uint32_t* dst, int version){
    void* tmp;
    tmp = dvan_buffer_consume(db, sizeof(*dst));
    if (!tmp) return -ENOMEM;
    *dst = ntohl(*((uint32_t*)tmp));
    return 0;
}

int dvan_buffer_consume_int32(dvan_buffer_t* db, int32_t* dst, int version){
    void* tmp;
    tmp = dvan_buffer_consume(db, sizeof(*dst));
    if (!tmp) return -ENOMEM;
    *dst = ntohl(*((int32_t*)tmp));
    return 0;
}

int dvan_buffer_consume_string(dvan_buffer_t* db, char** dst, int version){
    void* tmp;
    uint32_t len;
    int rc;
    if (!dst) return -EINVAL;
    rc = dvan_buffer_consume_uint32(db, &len, version);
    if (rc) return rc;
    if (len == 0){
        *dst = NULL;
        return 0;
    }
    tmp = dvan_buffer_consume(db, len);
    if (!tmp) return -ENOMEM;
    *dst = malloc(len * sizeof(char) +1);
    if (!*dst) return -ENOMEM;
    memcpy(*dst, tmp, len); 
    (*dst)[len] = '\0';
    return 0;    
}

void* dvan_buffer_consume(dvan_buffer_t* db, size_t len){
    void* rv;
    if (!db) return NULL;
    if (len <= 0) return NULL;
    if ((db->data_off + len) > db->data_len) return NULL;
    rv = db->buffer + db->data_off;
    db->data_off += len;
    return rv;
}

void dvan_buffer_consume_reset(dvan_buffer_t* db){
    if (!db) return;
    db->data_off = 0;
}

int dvan_buffer_consume_finalize(dvan_buffer_t* db){
    if (!db) return -EINVAL;
    if (db->data_off > db->data_len) return -EINVAL;
    memmove(db->buffer, db->buffer + db->data_off, db->data_len - db->data_len);
    db->data_len -= db->data_off;
    return 0; 
}

int dvan_buffer_isempty(dvan_buffer_t* db){
    return (db)?(db->data_len == 0):-EINVAL;
}


int dvan_buffer_from_socket(dvan_buffer_t* db, int sk){
    size_t avail;
    size_t bytes_read;
    if (!db) return -EINVAL;

    if (!dvan_buffer_available(db))
        dvan_buffer_extend(db);

    if ((avail = dvan_buffer_available(db)) == 0)
        return -ENOMEM;

    bytes_read = read(sk, db->buffer + db->data_len, avail); 

    if (bytes_read <= 0)
        return bytes_read;

    db->data_len += bytes_read;

    return bytes_read;
}

int dvan_buffer_to_socket(dvan_buffer_t* db, int sk){
    ssize_t bytes_written;

    if (!db) return -EINVAL;

    bytes_written = write(sk, db->buffer, db->data_len);
    if (bytes_written <= 0)
        return bytes_written;
    
    dvan_buffer_consume(db, bytes_written);
    dvan_buffer_consume_finalize(db);
    return bytes_written;
}

