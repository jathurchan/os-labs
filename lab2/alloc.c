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
void* breakPtr = NULL;


// ---- Helper functions ----

HEADER_TAG* getFreeBlock(size_t memorySize) {

    if(!breakPtr)   // Initialize breakPtr
        breakPtr = sbrk(0);

    if (breakPtr == (void*) -1) {   // Not enough memory
        printf("ERROR : SBRK AILED");
        exit(EXIT_FAILURE);
    }

    HEADER_TAG* currentPtr = freeMemoryBlockHead;

    while (currentPtr != NULL) {
        if (currentPtr->bloc_size >= memorySize)
            return currentPtr;
        currentPtr = currentPtr->ptr_next;
    }

    // Extend allocated free memory
    breakPtr = (void*) ((size_t)(breakPtr + memorySize + headerTagSize + magicNumberSize)); // update the stored value of break program

    void* headerPtr = sbrk((size_t)breakPtr + memorySize + headerTagSize + magicNumberSize);
    if (headerPtr == (void*) -1) {  // Not enough memory
        printf("ERROR : SBRK FAILED");
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
    int* ptr = malloc_3is(sizeof(int));
    *ptr = 5;
    printf("%d", *ptr);
    return EXIT_SUCCESS;
}

