#include "shared_def.h"

int main() {
    int shmid = shmget(SHM_KEY, sizeof(FairData), 0666);
    if (shmid < 0) { perror("SHM error"); return 1; }
    FairData *shm = (FairData *)shmat(shmid, NULL, 0);

    int max_batch, interval;
    printf("=== CUSTOMER GENERATOR ===\n");
    printf("Maximum person a batch: ");
    scanf("%d", &max_batch);
    printf("Customer interval (seconds): ");
    scanf("%d", &interval);

    int customer_id_counter = 1;
    srand(time(NULL));

    while (1) {
        sleep(interval);
        pthread_mutex_lock(&shm->lock);

        // Kiểm tra gian hàng Ticket-Check có đang ON không
        int ticket_active = (shm->stalls[0].status == 1) && 
                            (time(NULL) - shm->stalls[0].last_heartbeat <= HEARTBEAT_TIMEOUT);

        if (!ticket_active) {
            printf("[Generator] Ticket Check Stall closed! Customers cannot enter.\n");
            pthread_mutex_unlock(&shm->lock);
            continue;
        }

        int count = 1 + rand() % max_batch;
        for (int i = 0; i < count; i++) {
            Customer c;
            c.id = customer_id_counter++;
            c.enter_fair_time = time(NULL);
            c.enter_queue_time = time(NULL);
            memset(c.visited, 0, sizeof(c.visited));
            c.visited[0] = 1; 

            if (push_queue(&shm->stalls[0].in_queue, c)) {
                shm->fair_people_count++;
                printf("[Generator] Customer %d entered Ticket Queue.\n", c.id);
            }
        }

        pthread_mutex_unlock(&shm->lock);
    }
    return 0;
}