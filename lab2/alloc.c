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
void* breakPtr = sbrk(0);

void* getFreeBlock(size_t memorySize) {
    HEADER_TAG* loopPtr = freeMemoryBlockHead;
    while (loopPtr != NULL) {
        if (loopPtr->bloc_size >= memorySize) :
            return loopPtr;
        loopPtr = loopPtr->ptr_next;
    }
    breakPtr = breakPtr + memorySize + headerTagSize + magicNumberSize;
    return sbrk(breakPtr + memorySize + headerTagSize + magicNumberSize);
}

void* malloc_3is(size_t memoryBlockSize ) {
    void* headerPtr = getFreeBlock(memoryBlockSize);
    HEADER_TAG* castedHeaderPtr = (HEADER_TAG*) headerPtr;
    *castedHeaderPtr = HEADER_TAG(NULL, memoryBlockSize, magicNumber);
    (*magicNumber) *(headerPtr + memoryBlockSize + headerTagSize) = magicNumber;
    return headerPtr + headerTagSize;
}

void* free_3is(void* memoryBlockPtr) {
    HEADER_TAG* headerPtr = (*HEADER_TAG) (memoryBlockPtr - headerTagSize);
    magicNumber* magicNumberPtr = (magicNumber*) (memoryBlockPtr + headerPtr->bloc_size);

    if (headerPtr->magic_number != magicNumber || *magicNumberPtr != magicNumber)
        printf("WARNING : OUT OF BOUNCE");

    HEADER_TAG* loopPtr = freeMemoryBlockHead;
    while (loopPtr != NULL) && (loopPtr->ptr_next < headerPtr )
        loopPtr = loopPtr->ptr_next;

    loopPtr->ptr_next = headerPtr;
    headerPtr->ptr_next = loopPtr->ptr_next;
}