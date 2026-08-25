#include "shared_def.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>

int main() {
    int shmid;
    MicrowaveData *shm;
    int interval;

    srand(time(NULL));

    if ((shmid = shmget(SHM_KEY, SHMSIZ, 0666)) < 0) {
        perror("shmget error (Auto Door)");
        return 1;
    }

    if ((shm = (MicrowaveData *)shmat(shmid, NULL, 0)) == (MicrowaveData *)-1) {
        perror("shmat error");
        return 1;
    }

    printf("=== AUTO RANDOM DOOR TESTER ===\n");
    printf("Set interval of door (seconds):\n");


    if (scanf("%d", &interval) != 1 || interval <= 0) {
        printf("Not change. Exit.\n");
        shmdt(shm);
        return 0;
    }

    while (1) {
        sleep(interval);

        // Sinh số ngẫu nhiên từ 0 đến 4 (5 giá trị)
        // 0 -> OPEN (xác suất 1/5 = 20%)
        // 1, 2, 3, 4 -> CLOSE (xác suất 4/5 = 80%)
        int r = rand() % 5;

        if (r == 0) { // 1/5 -> MO CUA (OPEN)
            shm->door = 1;
            printf("[Auto Door]  (Door = OPEN)\n");
        } else { // 4/5 -> DONG CUA (CLOSE)
            shm->door = 0;
            printf("[Auto Door] (Door = CLOSE)\n");
        }
    }

    shmdt(shm);
    return 0;
}