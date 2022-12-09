#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


// --- Definitions ----

typedef struct HEADER_TAG {
    struct HEADER_TAG* ptr_next;
    size_t bloc_size;
    long magic_number; // 0x0123456789ABCDEF
} HEADER_TAG;

#define headerTagSize sizeof(HEADER_TAG)
#define magicNumber (long) 0x0123456789ABCDEF
#define magicNumberSize sizeof(magicNumber)


// ---- Global variables ----

HEADER_TAG* freeMemoryBlockHead = NULL;

// ---- Helper functions ----

HEADER_TAG* getFreeBlock(size_t memorySize) {

    HEADER_TAG* currentPtr = freeMemoryBlockHead;

    while (currentPtr != NULL) {
        if (currentPtr->bloc_size >= memorySize)
            return currentPtr;
        currentPtr = currentPtr->ptr_next;
    }

    // Extend allocated free memory

    void* headerPtr = sbrk((size_t) memorySize + headerTagSize + magicNumberSize);
    if (headerPtr == (void*) -1) {  // Not enough memory
        printf("ERROR : SBRK FAILED\n");
        exit(EXIT_FAILURE);
    }

    return (HEADER_TAG*) headerPtr;
}


// ---- Malloc and Free ----

void* malloc_3is(size_t memoryBlockSize) {
    
    HEADER_TAG* headerPtr = getFreeBlock(memoryBlockSize);
    
    // Initialization
    headerPtr->ptr_next = NULL;
    headerPtr->bloc_size = memoryBlockSize;
    headerPtr->magic_number = magicNumber;

    *((long*) (headerPtr + memoryBlockSize + headerTagSize)) = magicNumber;

    return headerPtr + headerTagSize;
}

void free_3is(void* memoryBlockPtr) {

    HEADER_TAG* headerPtr = memoryBlockPtr - headerTagSize;
    long* magicNumberPtr = memoryBlockPtr + headerPtr->bloc_size;

    if (headerPtr->magic_number != magicNumber || *magicNumberPtr != magicNumber) {
        printf("WARNING : OUT OF BOUNDS\n");
        exit(EXIT_FAILURE);
    }

    HEADER_TAG* currentPtr = freeMemoryBlockHead;
    while ((currentPtr != NULL) && (currentPtr->ptr_next != NULL) && (currentPtr->ptr_next < headerPtr))
        currentPtr = currentPtr->ptr_next;

    if (currentPtr == NULL) {   // No other free block
        freeMemoryBlockHead = headerPtr;
        headerPtr->ptr_next = NULL;
    }
    else {
        headerPtr->ptr_next = currentPtr->ptr_next;
        currentPtr->ptr_next = headerPtr;
    }

}

struct test {
    long a;
    long b;
    long c;
    long d;
    long e;
};

int main() {
    struct test* ptr = malloc_3is(sizeof(struct test));
    ptr->e = 10;
    printf("%ld", ptr->e);
    free_3is(ptr);
    printf("%ld", ptr->e);
    return EXIT_SUCCESS;
}

