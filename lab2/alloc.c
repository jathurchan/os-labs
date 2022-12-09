#include <stdlib.h>

typedef struct HEADER_TAG {
    struct HEADER_TAG* ptr_next; // pointe sur le prochain bloc libre
    size_t bloc_size; // taille du memory bloc en octets
    long magic_number; // 0x0123456789ABCDEFL
} HEADER;

void* malloc_3is(size_t memorySize ) {

}

void* free_3is(size_t memorySize) {

}