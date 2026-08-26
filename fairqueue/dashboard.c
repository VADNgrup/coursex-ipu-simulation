#include "shared_def.h"

int main() {
    int shmid = shmget(SHM_KEY, sizeof(FairData), 0666);
    if (shmid < 0) { perror("SHM error"); return 1; }
    FairData *shm = (FairData *)shmat(shmid, NULL, 0);

    while (1) {
        printf("\033[H\033[J"); // Clear Screen
        pthread_mutex_lock(&shm->lock);

        printf("========================================================================\n");
        printf("                        FAIR REAL-TIME DASHBOARD                        \n");
        printf("========================================================================\n");
        printf(" Total People Currently in Fair: %d\n", shm->fair_people_count);
        printf("------------------------------------------------------------------------\n");
        printf("%-16s | %-8s | %-7s | %-9s | %-10s | %-10s\n", 
               "Stall Name", "Status", "InQueue", "TotalDone", "AvgProcess", "AvgWaitTime");
        printf("------------------------------------------------------------------------\n");

        for (int i = 0; i < 4; i++) {
            Stall *s = &shm->stalls[i];
            int is_active = (s->status == 1) && (time(NULL) - s->last_heartbeat <= HEARTBEAT_TIMEOUT);

            double avg_p = (s->total_processed > 0) ? (s->total_process_time / s->total_processed) : 0.0;
            double avg_w = (s->total_processed > 0) ? (s->total_wait_time / s->total_processed) : 0.0;

            printf("%-16s | %-8s | %-7d | %-9d | %-9.2fs | %-9.2fs\n",
                   s->name,
                   is_active ? "ON" : "OFF",
                   s->in_queue.count,
                   s->total_processed,
                   avg_p,
                   avg_w);
        }
        printf("========================================================================\n");

        pthread_mutex_unlock(&shm->lock);
        sleep(1);
    }
    return 0;
}