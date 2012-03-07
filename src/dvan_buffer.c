#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
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
    
    memmove(db->buffer, db->buffer + bytes_written, db->data_len - bytes_written);
    db->data_len -= bytes_written;

    return bytes_written;
}

