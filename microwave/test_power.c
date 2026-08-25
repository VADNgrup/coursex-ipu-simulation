#include "shared_def.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

int main() {
    int shmid;
    MicrowaveData *shm;
    int interval;

    if ((shmid = shmget(SHM_KEY, SHMSIZ, 0666)) < 0) {
        perror("shmget error (Auto Power)");
        return 1;
    }

    if ((shm = (MicrowaveData *)shmat(shmid, NULL, 0)) == (MicrowaveData *)-1) {
        perror("shmat error");
        return 1;
    }

    printf("=== AUTO POWER BUTTON TESTER ===\n");
    printf("Set interval (second):\n");

    if (scanf("%d", &interval) != 1 || interval <= 0) {
        printf("Exit program.\n");
        shmdt(shm);
        return 0;
    }


    while (1) {
        sleep(interval);
        int r = rand() % 5;
        if (r == 0) { // 1/5 -> Plug out electric
            shm->elec = 0;
            printf("[Auto Power] (Plug out)\n");
        } else { // 4/5 -> Plug in electric
            shm->elec = 1;
            printf("[Auto Power] (Plug in)\n");
        }
    }

    shmdt(shm);
    return 0;
}