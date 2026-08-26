#include "sharedef.c"

int main() {
    int shmid = shmget(SHM_KEY, sizeof(FairData), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget error");
        return 1;
    }

    FairData *shm = (FairData *)shmat(shmid, NULL, 0);
    memset(shm, 0, sizeof(FairData));

    // init Mutex for Process-Shared
    // The `pthread_mutex_lock` function is used to acquire a mutex (mutual exclusion lock) in order to establish a critical section. 
    // This mechanism ensures that only one process is permitted to read from or write to shared memory at any given time, 
    // thereby preventing data conflicts (race conditions).
    // If another process has already called the lock, the current process will wait until the lock is released.
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    // By default, pthread_mutex only works between threads within the same process. 
    // To make this mutex effective across multiple independent processes accessing shared memory, 
    // the following property is set in the init_shm.c file:
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shm->lock, &attr);

    // init stall
    const char *names[4] = {"Ticket Check", "Chicken Stall", "Takoyaki Stall", "Yakiniku Stall"};
    for (int i = 0; i < 4; i++) {
        strcpy(shm->stalls[i].name, names[i]);
        shm->stalls[i].is_optional = (i == 0) ? 0 : 1;
        shm->stalls[i].status = 0;
        init_queue(&shm->stalls[i].in_queue);
        init_queue(&shm->stalls[i].out_queue);
    }

    printf("Shared Memory initialized successfully!\n");
    shmdt(shm);
    return 0;
}