#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

char* fileName = "text.txt";

int main () {

    int openFileDescriptor, fileSize;
    char* mapPointer;
    struct stat statBuffer;

    if ((openFileDescriptor = open(fileName, O_RDWR, 0)) == -1)
        exit(EXIT_FAILURE);
    if (fstat(openFileDescriptor, &statBuffer) == -1)
        exit(EXIT_FAILURE);

    fileSize = (int) statBuffer.st_size;

    if ((mapPointer = mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, openFileDescriptor, 0)) == MAP_FAILED)
        exit(EXIT_FAILURE);

    char tmp;
    for (int i=0; i < (fileSize-1)/2; i++) {
        tmp = mapPointer[i];
        mapPointer[i] = mapPointer[fileSize-i-1];
        mapPointer[fileSize-i-1] = tmp;
    }

    if (munmap(mapPointer, fileSize) == -1)
        exit(EXIT_FAILURE);

    exit(EXIT_SUCCESS);
}