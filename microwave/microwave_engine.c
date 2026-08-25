#include "shared_def.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

const char* get_state_name(SystemState s) {
    switch(s) {
        case POWER_OFF: return "POWER_OFF";
        case IDLE:      return "IDLE";
        case DOOR_OPEN: return "DOOR_OPEN";
        case COOKING:   return "COOKING";
        case PAUSED:    return "PAUSED";
        case FINISHED:  return "FINISHED";
        default:        return "UNKNOWN";
    }
}

void reset_hardware_when_power_on(MicrowaveData *shm){
    shm->emitter = 0;
    shm->bell = 0;
    shm->light = 0;
}

void reset_hardware_when_power_off(MicrowaveData *shm){
    shm->timer = 0;
    shm->emitter = 0;
    shm->bell = 0;
    shm->light = 0;
}



int main() {
    int shmid;
    int mode = 1;
    MicrowaveData *shm;

    printf("Set mode:\n");
    printf("1: Clean screen (Dashboard view)\n");
    printf("2: Tracking screen (Log view)\n");
    printf("-> ");
    if (scanf("%d", &mode) != 1) mode = 1;

    if ((shmid = shmget(SHM_KEY, SHMSIZ, 0666)) < 0) {
        perror("shmget error (Engine)");
        return 1;
    }

    if ((shm = (MicrowaveData *)shmat(shmid, NULL, 0)) == (MicrowaveData *)-1) {
        perror("shmat error");
        return 1;
    }
    int count = 0;
    while(1) {
        count++;
        // =========================================================
        // BƯỚC 1: CHECK COMMAND (Chỉ cập nhật tham số thô vào Shared Memory)
        // =========================================================
        if (shm->cmd != 0) {
            int command = shm->cmd;
            shm->cmd = 0; // Clear command ngay lập tức

            switch(command) {
                case 1: // Toggle Power
                    shm->elec = !shm->elec;
                    break;

                case 2: // Toggle Door
                    shm->door = !shm->door;
                    break;

                case 3: // Tăng time (+30s)
                    if (shm->elec) {
                        shm->timer += 10;
                        if (shm->timer > 600) shm->timer = 600;
                    }
                    break;

                case 4: // Giảm time (-30s)
                    if (shm->elec) {
                        shm->timer = (shm->timer >= 50) ? (shm->timer - 50) : 0;
                    }
                    break;

                case 5: // Đổi công suất (500W <-> 700W)
                    if (shm->elec) {
                        shm->power = (shm->power == 500) ? 700 : 500;
                    }
                    break;

                case 6: // Nút START
                    if (shm->elec && !shm->door && shm->timer > 0) {
                        shm->state = COOKING;
                        reset_hardware_when_power_on(shm);
                    }
                    break;

                case 7: // Nút STOP / CLEAR
                    if (shm->elec) {
                        if (shm->state == COOKING) {
                            shm->state = PAUSED;
                        }
                    }
                    break;
            }
        }

        // =========================================================
        // BƯỚC 2: CHECK LOGIC (Cập nhật State Machine & Ngoại vi)
        // =========================================================
        
        // 2.1. Xử lý nguồn điện
        if (!shm->elec) {
            reset_hardware_when_power_off(shm);
            shm->state = POWER_OFF;
        } else {
            // Khi mở cửa
            reset_hardware_when_power_on(shm);
            if (shm->door) {
                shm->state = DOOR_OPEN;
            }  else {
                // Khi đóng cửa
                // Đếm lùi thời gian & chuyển về FINISHED khi hết giờ
                if (shm->state == COOKING) {
                    if (shm->timer > 0) {
                        shm->timer--;
                        shm->light = 1;
                        shm->emitter = 1;  
                    }
                    if (shm->timer == 0) {
                        shm->state = FINISHED;
                        shm->bell = 1;
                    }
                }
                else if (shm->state == FINISHED && count % 3 == 0) {
                        shm->bell = 1;
                }
                else if (shm->state != PAUSED && shm->state != FINISHED) {shm->state = IDLE;}
                
            }


        }


        // =========================================================
        // 3: PRINTOUT  
        // =========================================================
        if (mode == 1) {
            printf("\033[H\033[J"); // clean screen
        }

        printf("\n=========================================\n");
        printf("       MICROWAVE OVEN SIMULATION         \n");
        printf("=========================================\n");
        printf(" Electricity : %s\n", shm->elec ? "ON 🔌" : "OFF ❌");
        printf(" Door        : %s\n", shm->door ? "OPEN 🔓" : "CLOSE 🔒");
        printf(" Timer       : %d s ⌛\n", shm->timer);
        printf(" Power       : %d W\n", shm->power);
        printf("-----------------------------------------\n");
        printf(" State       : %s\n", get_state_name(shm->state));
        printf(" Light: %-3s | Emitter: %-3s | Bell: %-3s\n",
               shm->light ? "💡" : "❌",
               shm->emitter ? (count % 2 == 0 ? "🥘" : "🍲") : "❌",
               shm->bell ? "RING 🔔" : "🔇");
        printf("=========================================\n");
        fflush(stdout);

        sleep(1);
    }

    shmdt(shm);
    return 0;
}