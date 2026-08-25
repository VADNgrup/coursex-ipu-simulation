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
        perror("shmget error (Auto Start)");
        return 1;
    }

    if ((shm = (MicrowaveData *)shmat(shmid, NULL, 0)) == (MicrowaveData *)-1) {
        perror("shmat error");
        return 1;
    }

    printf("=== AUTO START BUTTON TESTER ===\n");
    printf("Set interval (second):\n");

    if (scanf("%d", &interval) != 1 || interval <= 0) {
        shmdt(shm);
        return 0;
    }


    while (1) {
        sleep(interval);
        int r = rand() % 5;

        if (r == 0) { // 1/5 -> MO CUA (OPEN)
            shm->cmd = 6;
            printf("Start");
        } else { // 4/5 -> DONG CUA (CLOSE)
            shm->cmd = 7;
            printf("Stop");
        }
        // Gửi lệnh START (cmd = 6)
    }

    shmdt(shm);
    return 0;
}