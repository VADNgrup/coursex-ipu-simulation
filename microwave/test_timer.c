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

    // Khởi tạo seed cho hàm random dựa trên thời gian thực
    srand(time(NULL));

    if ((shmid = shmget(SHM_KEY, SHMSIZ, 0666)) < 0) {
        perror("shmget error (Auto Timer)");
        return 1;
    }

    if ((shm = (MicrowaveData *)shmat(shmid, NULL, 0)) == (MicrowaveData *)-1) {
        perror("shmat error");
        return 1;
    }

    printf("=== AUTO TIMER RANDOM TESTER ===\n");
    printf("Set interval (second):\n");

    if (scanf("%d", &interval) != 1 || interval <= 0) {
        shmdt(shm);
        return 0;
    }


    while (1) {
        sleep(interval);
        // Sinh giá trị ngẫu nhiên từ 0 đến 600 giây
        int random_timer = rand() % 601;
        shm->timer = random_timer;
        printf("Set timer = %d\n", random_timer);
    }

    shmdt(shm);
    return 0;
}