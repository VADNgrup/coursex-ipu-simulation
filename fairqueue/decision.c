#include "shared_def.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <from_stall_id (0:Ticket, 1:Chicken, 2:Takoyaki)>\n", argv[0]);
        return 1;
    }

    int from_stall = atoi(argv[1]);
    int shmid = shmget(SHM_KEY, sizeof(FairData), 0666);
    if (shmid < 0) { perror("SHM error"); return 1; }
    FairData *shm = (FairData *)shmat(shmid, NULL, 0);

    srand(time(NULL) + from_stall * 10);
    printf("=== DECISION ROUTER FOR STALL %d STARTED ===\n", from_stall);

    while (1) {
        pthread_mutex_lock(&shm->lock);

        Customer c;
        if (pop_queue(&shm->stalls[from_stall].out_queue, &c)) {
            // Tìm các quầy Optional (1, 2, 3) đang MỞ và CHƯA GHÉ THĂM
            int available_stalls[3];
            int avail_count = 0;

            for (int i = 1; i < 4; i++) {
                int is_active = (shm->stalls[i].status == 1) && 
                                (time(NULL) - shm->stalls[i].last_heartbeat <= HEARTBEAT_TIMEOUT);
                
                if (is_active && !c.visited[i]) {
                    available_stalls[avail_count++] = i;
                }
            }

            // Tỉ lệ 25% tự nguyện đi về hoặc không còn quầy nào mở/chưa đi
            if (avail_count == 0 || (rand() % 100 < 25)) {
                shm->fair_people_count--;
                printf("[Decision] Customer %d LEFT the fair.\n", c.id);
            } else {
                // Chọn ngẫu nhiên 1 gian hàng khả dụng
                int next_stall = available_stalls[rand() % avail_count];
                c.visited[next_stall] = 1;
                c.enter_queue_time = time(NULL);
                
                push_queue(&shm->stalls[next_stall].in_queue, c);
                printf("[Decision] Customer %d -> Moving to %s\n", c.id, shm->stalls[next_stall].name);
            }
        }

        pthread_mutex_unlock(&shm->lock);
        usleep(200000);
    }
    return 0;
}