#include <stdlib.h>

typedef struct HEADER_TAG {
    struct HEADER_TAG* ptr_next; // pointe sur le prochain bloc libre
    size_t bloc_size; // taille du memory bloc en octets
    long magic_number; // 0x0123456789ABCDEFL
} HEADER_TAG;

#define headerTagSize sizeof(HEADER_TAG)
#define magicNumberSize (size_t) 16
#define magicNumber 0x01234568ABCDEF

HEADER_TAG* freeMemoryBlockHead = NULL;

void* getFreeBlock(size_t memorySize) {
    HEADER_TAG* loopPtr = freeMemoryBlockHead;
    while (loopPtr != NULL) {
        if (loopPtr->bloc_size >= memorySize)
            return loopPtr;
        loopPtr = loopPtr->ptr_next;
    }
    return sbrk(sbrk(0) + memorySize + headerTagSize + magicNumberSize);
}

void* malloc_3is(size_t memorySize ) {
    void* headerPtr = getFreeBlock(memorySize);
    (*HEADER_TAG) *headerPtr= HEADER_TAG(NULL, memorySize, magicNumber);
    (*magicNumber) *(headerPtr + memorySize + headerTagSize) = magicNumber;
    return headerPtr + headerTagSize;
}

void* free_3is(size_t memorySize) {

}