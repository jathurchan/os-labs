#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>


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
size_t preallocatedMemory = 132000;
bool memoryPreallocated = false;

// ---- Helper functions ----

HEADER_TAG* getFreeBlock(size_t memorySize) {

    HEADER_TAG* currentPtr = freeMemoryBlockHead;

    if (currentPtr != NULL && currentPtr->ptr_next == NULL) {
        if (currentPtr->bloc_size >= memorySize+headerTagSize+magicNumberSize+1) {
            HEADER_TAG* dividedHeaderPtr = (HEADER_TAG*) ((void*) currentPtr + memorySize + headerTagSize + magicNumberSize);
            dividedHeaderPtr->ptr_next = NULL;
            dividedHeaderPtr->bloc_size = currentPtr->bloc_size - memorySize - headerTagSize + magicNumberSize;
            freeMemoryBlockHead = dividedHeaderPtr;
            return currentPtr;
        }
        else if (currentPtr->bloc_size >= memorySize) {
            freeMemoryBlockHead = NULL;
            return currentPtr;
        }
    }

    while (currentPtr != NULL && currentPtr->ptr_next != NULL) {
        if (currentPtr->ptr_next->bloc_size >= memorySize+headerTagSize+magicNumberSize+1) {
            HEADER_TAG* dividedHeaderPtr = (HEADER_TAG*) ((void*) (currentPtr->ptr_next) + memorySize + headerTagSize + magicNumberSize);
            HEADER_TAG* returnedPtr = currentPtr->ptr_next;
            dividedHeaderPtr->ptr_next = currentPtr->ptr_next->ptr_next;
            dividedHeaderPtr->bloc_size = currentPtr->bloc_size - memorySize - headerTagSize - magicNumberSize;
            currentPtr->ptr_next = dividedHeaderPtr;
            return returnedPtr;
        }
        else if (currentPtr->ptr_next->bloc_size >= memorySize) {
            HEADER_TAG* returnedPtr = currentPtr->ptr_next;
            currentPtr->ptr_next = currentPtr->ptr_next->ptr_next;
            return returnedPtr;
        }
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

void adjacentBlocksFusion() {
    HEADER_TAG* currentPtr = freeMemoryBlockHead;
    while (currentPtr != NULL && currentPtr->ptr_next != NULL) {
        if ((void*)currentPtr + currentPtr->bloc_size + headerTagSize + magicNumberSize == currentPtr->ptr_next) {
            currentPtr->bloc_size = currentPtr->bloc_size + headerTagSize + magicNumberSize + currentPtr->ptr_next->bloc_size;
            currentPtr->ptr_next = currentPtr->ptr_next->ptr_next;
        }
        else
            currentPtr = currentPtr->ptr_next;
    }
}

void memoryPreallocation() {
    void* preallocatedMemoryPtr = (HEADER_TAG*) getFreeBlock(preallocatedMemory);
    freeMemoryBlockHead = (HEADER_TAG*) preallocatedMemoryPtr;
    freeMemoryBlockHead->magic_number = magicNumber;
    *((long*) (preallocatedMemoryPtr + preallocatedMemory + headerTagSize)) = magicNumber;
    freeMemoryBlockHead->ptr_next = NULL;
    freeMemoryBlockHead->bloc_size = preallocatedMemory;

    if (freeMemoryBlockHead->magic_number != magicNumber || *((long*) (preallocatedMemoryPtr + preallocatedMemory + headerTagSize)) != magicNumber) {
        printf("WARNING : PREALLOCATION OUT OF BOUNDS\n");
        exit(EXIT_FAILURE);
    }

    memoryPreallocated = true;
}

// ---- Malloc and Free ----

void* malloc_3is(size_t memoryBlockSize) {

    //Preallocate memory the first time malloc is called
    if (!memoryPreallocated) {
        memoryPreallocation();
    }

    void* headerPtr = getFreeBlock(memoryBlockSize);

    HEADER_TAG* castedHeaderPtr = (HEADER_TAG*) headerPtr;

    // Initialization
    castedHeaderPtr->magic_number = magicNumber;
    *((long*) (headerPtr + memoryBlockSize + headerTagSize)) = magicNumber;
    castedHeaderPtr->ptr_next = NULL;
    castedHeaderPtr->bloc_size = memoryBlockSize;

    if (castedHeaderPtr->magic_number != magicNumber || *((long*) (headerPtr + memoryBlockSize + headerTagSize)) != magicNumber) {
        printf("WARNING : OUT OF BOUNDS\n");
        exit(EXIT_FAILURE);
    }

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

    adjacentBlocksFusion();
}