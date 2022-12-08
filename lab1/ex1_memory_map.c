#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/mman.h>

int globalVariable = 1;
int uninitializedGlobalVariable;


int main() {

    // Data
    printf("Data Address: %p\n", &globalVariable);

    // BSS
    printf("BSS Address: %p\n", &uninitializedGlobalVariable);

    // Str
    char* string;
    printf("String Address: %p\n", &string);

    // Heap
    int *dynamicInt = malloc(sizeof(int)*4);
    printf("Heap Address: %p\n", dynamicInt);
    int *dynamicLargeInt = malloc(sizeof(int)*(1<<20));
    printf("Heap (Large) Address: %p\n", dynamicLargeInt);

    // Stack
    int localVarible;
    printf("Stack Address: %p\n", &localVarible);

    // Main Function
    printf("Main Function Address: %p\n", main);
    printf("Main Function Address with &: %p\n", &main);

    // LibC Function
    printf("LibC Function (printf) Address: %p\n", printf);

    // Mmap
    int *mmapExample = mmap(NULL, 5*sizeof(int), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    printf("Mmap Address: %p\n", mmapExample);

    // Get Memory Map

    int pid, status;

    pid = fork();

    if (pid != 0) {

        wait(&status);

    } else {

        char* ppid = malloc(sizeof(int));
        sprintf(ppid, "%d", getppid());
        execlp("/usr/bin/vmmap", "vmmap", ppid, (char *) NULL); // MacOS specific command
        
        perror("execlp failed!");
        
    }

    return EXIT_SUCCESS;
}