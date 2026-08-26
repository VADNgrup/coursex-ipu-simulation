#include "shared_def.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <stall_id (0:Ticket, 1:Chicken, 2:Takoyaki, 3:Yakiniku)>\n", argv[0]);
        return 1;
    }

    int stall_id = atoi(argv[1]);
    int shmid = shmget(SHM_KEY, sizeof(FairData), 0666);
    if (shmid < 0) { perror("SHM error"); return 1; }
    FairData *shm = (FairData *)shmat(shmid, NULL, 0);

    srand(time(NULL) + stall_id);
    printf("=== STALL WORKER: %s STARTED ===\n", shm->stalls[stall_id].name);

    while (1) {
        pthread_mutex_lock(&shm->lock);

        // Gửi Heartbeat báo hiệu Stall đang hoạt động
        shm->stalls[stall_id].status = 1;
        shm->stalls[stall_id].last_heartbeat = time(NULL);

        Customer c;
        int has_customer = pop_queue(&shm->stalls[stall_id].in_queue, &c);
        pthread_mutex_unlock(&shm->lock);

        if (has_customer) {
            double wait_time = difftime(time(NULL), c.enter_queue_time);
            
            // Random thời gian xử lý (từ 1s đến 3s)
            int process_time = 1 + rand() % 3;
            sleep(process_time);

            pthread_mutex_lock(&shm->lock);
            shm->stalls[stall_id].total_processed++;
            shm->stalls[stall_id].total_process_time += process_time;
            shm->stalls[stall_id].total_wait_time += wait_time;

            // Đẩy khách sang out_queue để chương trình Decision xử lý
            push_queue(&shm->stalls[stall_id].out_queue, c);
            pthread_mutex_unlock(&shm->lock);

            printf("[%s] Processed Customer %d (Wait: %.0fs, Service: %ds)\n", 
                   shm->stalls[stall_id].name, c.id, wait_time, process_time);
        } else {
            usleep(200000); // 200ms
        }
    }
    return 0;
}