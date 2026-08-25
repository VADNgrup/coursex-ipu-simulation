#include "shared_def.h"

const char* get_state_name(SystemState s) {
    switch(s) {
        case POWER_OFF: return "POWER_OFF";
        case IDLE: return "IDLE";
        case DOOR_OPEN: return "DOOR_OPEN";
        case COOKING: return "COOKING";
        case PAUSED: return "PAUSED";
        case FINISHED: return "FINISHED";
        default: return "UNKNOWN";
    }
}

int main() {
    int shmid;
    MicrowaveData *shm;

    if ((shmid = shmget(SHM_KEY, SHMSIZ, 0666)) < 0) {
        perror("shmget error (Engine)");
        return 1;
    }

    if ((shm = (MicrowaveData *)shmat(shmid, NULL, 0)) == (MicrowaveData *)-1) {
        perror("shmat error");
        return 1;
    }

    while(1) {
        // 1. Xử lý các lệnh từ Controller
        if (shm->cmd != 0) {
            int command = shm->cmd;
            shm->cmd = 0; 

            switch(command) {
                case 1: // Toggle Nguồn điện
                    shm->elec = !shm->elec;
                    if (!shm->elec) {
                        shm->state = POWER_OFF;
                        shm->timer = 0;
                    } else {
                        shm->state = shm->door ? DOOR_OPEN : IDLE;
                    }
                    break;

                case 2: // Toggle Cửa (ĐỘC LẬP - Chạy được cả khi POWER_OFF)
                    shm->door = !shm->door;
                    if (shm->elec) {
                        if (shm->door) { // Mở cửa
                            if (shm->state == COOKING) shm->state = PAUSED;
                            else if (shm->state == IDLE || shm->state == FINISHED) shm->state = DOOR_OPEN;
                        } else { // Đóng cửa
                            if (shm->state == DOOR_OPEN) shm->state = IDLE;
                        }
                    }
                    break;

                case 3: // Tăng thời gian (+30s)
                    if (shm->elec && shm->state != POWER_OFF) {
                        shm->timer += 30;
                        if (shm->timer > 600) shm->timer = 600;
                    }
                    break;

                case 4: // Giảm thời gian (-30s / vặn núm giảm timer)
                    if (shm->elec && shm->state != POWER_OFF) {
                        if (shm->timer >= 30) shm->timer -= 30;
                        else shm->timer = 0;

                        // Nếu vặn về 0 lúc đang COOKING -> Dừng Emitter & Rung Bell
                        if (shm->timer == 0 && shm->state == COOKING) {
                            shm->state = FINISHED;
                        }
                    }
                    break;

                case 5: // Đổi công suất (500W <-> 700W)
                    if (shm->elec) {
                        shm->power = (shm->power == 500) ? 700 : 500;
                    }
                    break;

                case 6: // Nút Start
                    if (shm->elec && !shm->door && shm->timer > 0) {
                        shm->state = COOKING;
                    }
                    break;

                case 7: // Nút Stop / Clear
                    if (shm->elec) {
                        shm->timer = 0;
                        shm->state = shm->door ? DOOR_OPEN : IDLE;
                    }
                    break;
            }
        }

        // 2. Logic đếm lùi thời gian khi đang COOKING
        if (shm->elec && shm->state == COOKING) {
            if (shm->timer > 0) {
                shm->timer--;
            }
            if (shm->timer == 0) {
                shm->state = FINISHED; // Hết giờ -> Chuyển sang FINISHED
            }
        }

        // 3. Cập nhật thiết bị ngoại vi (Outputs)
        if (!shm->elec) {
            shm->state = POWER_OFF;
            shm->timer = 0;
            shm->light = 0;
            shm->emitter = 0;
            shm->bell = 0;
        } else {
            // Light CHỈ bật khi CÓ ĐIỆN và CỬA ĐÓNG
            shm->light = (!shm->door) ? 1 : 0;
            
            // Emitter CHỈ bật khi COOKING
            shm->emitter = (shm->state == COOKING) ? 1 : 0;
            
            // Bell CHỈ kêu khi FINISHED
            shm->bell = (shm->state == FINISHED) ? 1 : 0;
        }

        // 4. Hiển thị Dashboard
        printf("\033[H\033[J");
        printf("=========================================\n");
        printf("       MICROWAVE OVEN SIMULATION         \n");
        printf("=========================================\n");
        printf(" State       : %s\n", get_state_name(shm->state));
        printf(" Electricity : %s\n", shm->elec ? "ON" : "OFF");
        printf(" Door        : %s\n", shm->door ? "OPEN" : "CLOSE");
        printf(" Timer       : %d s / 600 s\n", shm->timer);
        printf(" Power       : %d W\n", shm->power);
        printf("-----------------------------------------\n");
        printf(" [Outputs] Light: %-3s | Emitter: %-3s | Bell: %-3s\n",
               shm->light ? "ON" : "OFF",
               shm->emitter ? "ON" : "OFF",
               shm->bell ? "RING" : "OFF");
        printf("=========================================\n");
        fflush(stdout);

        sleep(1);
    }

    return 0;
}