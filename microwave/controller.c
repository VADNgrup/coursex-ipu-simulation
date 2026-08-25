#include "shared_def.h"

int main() {
    int shmid;
    MicrowaveData *shm;
    int choice;

    if ((shmid = shmget(SHM_KEY, SHMSIZ, 0666)) < 0) {
        perror("shmget error (Controller)");
        return 1;
    }

    if ((shm = (MicrowaveData *)shmat(shmid, NULL, 0)) == (MicrowaveData *)-1) {
        perror("shmat error");
        return 1;
    }

    while(1) {
        printf("\n--- CONTROLLER MENU ---\n");
        printf("1. Toggle Power (Plug/Unplug)\n");
        printf("2. Toggle Door (Open/Close)\n");
        printf("3. Add Time (+30s)\n");
        printf("4. Decrease Time (-30s)\n");
        printf("5. Switch Watt (500W/700W)\n");
        printf("6. Press START\n");
        printf("7. Press STOP\n");
        printf("0. Exit Controller\n");
        printf("Choose action -> ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            continue;
        }

        if (choice == 0) break;
        if (choice >= 1 && choice <= 7) {
            shm->cmd = choice;
            printf(">> Sent Command %d successfully.\n", choice);
        } else {
            printf("Invalid choice!\n");
        }
    }

    shmdt(shm);
    return 0;
}