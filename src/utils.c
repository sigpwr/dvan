#include <stdlib.h>
#include <stdio.h>

void* dvan_malloc(size_t sz){
    void* rv;
    rv = malloc(sz);
#ifdef DVAN_DEBUG
    if (rv)
        fprintf(stderr, "%16p: Allocated \t%010zu\n", rv, sz);
    else
        fprintf(stderr, "%16p: F T Allocate \t%010zu\n", NULL, sz);
#endif
    return rv;
}

void* dvan_realloc(void* ptr, size_t sz){
    void* rv;
    rv = realloc(ptr, sz);
#ifdef DVAN_DEBUG
    if (rv)
        fprintf(stderr, "%16p: Reallocated from \t%16p (size %010zu)\n", rv, ptr, sz);
    else
        fprintf(stderr, "%16p: F T Reallocate from \t%16p (size %010zu)\n", NULL, ptr, sz);
#endif
    return rv;
}

void dvan_free(void* ptr){
    free(ptr);
#ifdef DVAN_DEBUG
    fprintf(stderr, "%16p: Freed\n", ptr);
#endif
}

