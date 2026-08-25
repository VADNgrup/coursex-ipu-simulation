#include "shared_def.h"

int main() {
    int shmid;
    MicrowaveData *shm;

    if ((shmid = shmget(SHM_KEY, SHMSIZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget error");
        return 1;
    }

    if ((shm = (MicrowaveData *)shmat(shmid, NULL, 0)) == (MicrowaveData *)-1) {
        perror("shmat error");
        return 1;
    }

    // Khởi tạo trạng thái mặc định
    memset(shm, 0, sizeof(MicrowaveData));
    shm->elec = 1;         // Mặc định đã cắm điện
    shm->power = 500;      // Công suất mặc định 500W
    shm->state = IDLE;
    shm->cmd = 0;

    printf("Shared memory created and initialized. SHMID: %x\n", shmid);
    shmdt(shm);
    return 0;
}