#include "segdef.h"
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>

int semId, shmId, reqCount = 0, randSum = 0;
char *buf;
struct sembuf sop;
struct shmseg seg;

void initClt() {

    if ((shmId = shmget(cle, 0, 0)) == -1)
        exit(EXIT_FAILURE);

    if ((buf = shmat(shmId, 0, 0)) == (char*)-1)
        exit(EXIT_FAILURE);

    if ((semId = semget(cle, 0, 0) == -1))
        exit(EXIT_FAILURE);

    init_rand();
}

void initSeg() {
    seg.pid = getpid();
    seg.req = reqCount++;
    for (int i = 0; i < maxval; i++)
        randSum += (seg.tab[i] = getrand());
    seg.result = randSum/maxval;
}

void loopClt() {

    acq_sem(semId, seg_dispo);

    initSeg();
    wait_sem(semId, res_ok);

    printf("Result Client: %ld\n", seg.result);
    lib_sem(semId, seg_init);

    wait_sem(semId, res_ok);

    lib_sem(semId, seg_dispo);

    printf("Result Server: %ld\n", seg.result);
}

int main() {
    initClt();
    //while(1)
        loopClt();
}
