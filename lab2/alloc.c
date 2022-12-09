#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


typedef struct HEADER_TAG {
    struct HEADER_TAG* ptr_next; // pointe sur le prochain bloc libre
    size_t bloc_size; // taille du memory bloc en octets
    long magic_number; // 0x0123456789ABCDEF
} HEADER_TAG;

#define headerTagSize sizeof(HEADER_TAG)
#define magicNumber (long) 0x0123456789ABCDEF
#define magicNumberSize sizeof(magicNumber)

HEADER_TAG* freeMemoryBlockHead = NULL;
void* breakPtr = NULL;

void* getFreeBlock(size_t memorySize) {
    if(!breakPtr)
        breakPtr = sbrk(0);
    if (breakPtr == (void*) -1) {
        printf("ERROR : SBRK FAILED");
        exit(EXIT_FAILURE);
    }
    HEADER_TAG* loopPtr = freeMemoryBlockHead;
    while (loopPtr != NULL) {
        if (loopPtr->bloc_size >= memorySize)
            return loopPtr;
        loopPtr = loopPtr->ptr_next;
    }
    breakPtr = (void*) ((size_t)(breakPtr + memorySize + headerTagSize + magicNumberSize));
    return sbrk((size_t)breakPtr + memorySize + headerTagSize + magicNumberSize);
}

void* malloc_3is(size_t memoryBlockSize ) {
    void* headerPtr = getFreeBlock(memoryBlockSize);
    if (headerPtr == (void*) -1) {
        printf("ERROR : SBRK FAILED");
        exit(EXIT_FAILURE);
    }
    ((HEADER_TAG*) headerPtr)->ptr_next = NULL, ((HEADER_TAG*) headerPtr)->bloc_size = memoryBlockSize, ((HEADER_TAG*) headerPtr)->magic_number = magicNumber;
    *((long*) (headerPtr + memoryBlockSize + headerTagSize)) = magicNumber;
    return headerPtr + headerTagSize;
}

void free_3is(void* memoryBlockPtr) {
    HEADER_TAG* headerPtr = memoryBlockPtr - headerTagSize;
    long* magicNumberPtr = memoryBlockPtr + headerPtr->bloc_size;

    if (headerPtr->magic_number != magicNumber || *magicNumberPtr != magicNumber)
        printf("WARNING : OUT OF BOUNCE");

    HEADER_TAG* loopPtr = freeMemoryBlockHead;
    while ((loopPtr != NULL) && (loopPtr->ptr_next < headerPtr))
        loopPtr = loopPtr->ptr_next;

    if (loopPtr == NULL) {
        freeMemoryBlockHead = headerPtr;
        headerPtr->ptr_next = NULL;
    }
    else {
        loopPtr->ptr_next = headerPtr;
        headerPtr->ptr_next = loopPtr->ptr_next;
    }
}

int main() {

}

