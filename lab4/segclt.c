#include "segdef.h"
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <unistd.h>


struct sembuf sop;
struct shmseg seg;


// Global variables

int semId, shmId, reqCount = 0, randSum = 0, numLoop = 100000, numFork = 4;
char *buf;


// Initialization

void initClt() {

    if ((shmId = shmget(cle, 0, 0)) == -1)
        exit(EXIT_FAILURE);

    if ((buf = shmat(shmId, 0, 0)) == (char*)-1)
        exit(EXIT_FAILURE);

    if ((semId = semget(cle, 0, 0) == -1))
        exit(EXIT_FAILURE);

    init_rand();
}


// Loop controlled by the counter

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
    acq_sem(semId, seg_init);

    wait_sem(semId, res_ok);

    printf("Client result %d: %ld\n", seg.pid, seg.result);
    lib_sem(semId, seg_init);

    acq_sem(semId, res_ok);
    lib_sem(semId, res_ok);

    lib_sem(semId, seg_dispo);

    printf("Server result %d: %ld\n", seg.pid, seg.result);
}

int main() {
    while(numFork--)
        fork();
    initClt();
    while(numLoop--)
        loopClt();
    shmdt(buf);
    exit(0);
}


// Custom implementations

void wait_sem2(semid, sem) {
    sop.sem_num = sem;
    sop.sem_op = 0; // wait for 0
    sop.sem_flg = 0;

    if (semop(semid, &sop, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

void acq_sem2(semid, sem) {
    sop.sem_num = sem;
    sop.sem_flg = -1;   // acquire
    sop.sem_flg = IPC_NOWAIT;

    if (semop(semid, &sop, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

}

void lib_sem2(semid, sem) {
    sop.sem_num = sem;
    sop.sem_flg = 1;   // release
    sop.sem_flg = IPC_NOWAIT;

    if (semop(semid, &sop, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}