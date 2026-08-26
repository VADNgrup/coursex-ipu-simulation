#include "sharedef.h"

// Kitchen:
// - Lấy phần tử từ Q_FOOD
// - Sleep dựa trên khoảng thời gian của món ăn (Ramen / Kare)
// - Sau đó đẩy student vào Q_WAITING (chờ bàn)
int main() {
    int msgid_in  = msgget(ftok(".", KEY_FOOD),    0666 | IPC_CREAT);
    int msgid_out = msgget(ftok(".", KEY_WAITING), 0666 | IPC_CREAT);
    if (msgid_in < 0 || msgid_out < 0) { perror("MSGGET Error"); return 1; }

    CafeteriaConfig *cfg = get_shared_config();
    srand(time(NULL) + 2);
    printf("=== KITCHEN STARTED ===\n");
    printf("Waiting for orders in Q_FOOD...\n");

    while (1) {
        struct mesg_buffer msg;
        if (msgrcv(msgid_in, &msg, sizeof(Student), 0, 0) == -1) continue;

        double wait_time = difftime(time(NULL), msg.student.enter_queue_time);

        int ramen_min = cfg ? cfg->ramen_prep_min : DEFAULT_RAMEN_PREP_MIN;
        int ramen_max = cfg ? cfg->ramen_prep_max : DEFAULT_RAMEN_PREP_MAX;
        int kare_min  = cfg ? cfg->kare_prep_min  : DEFAULT_KARE_PREP_MIN;
        int kare_max  = cfg ? cfg->kare_prep_max  : DEFAULT_KARE_PREP_MAX;

        if (ramen_max < ramen_min) ramen_max = ramen_min;
        if (kare_max < kare_min) kare_max = kare_min;

        int prep_time;
        if (msg.student.food == FOOD_RAMEN) {
            prep_time = ramen_min + rand() % (ramen_max - ramen_min + 1);
        } else {
            prep_time = kare_min  + rand() % (kare_max  - kare_min  + 1);
        }
        sleep(prep_time);

        printf("[Kitchen] Student %d | %s done | Wait: %.0fs | Prep: %ds\n",
               msg.student.id, food_name(msg.student.food), wait_time, prep_time);

        write_log("KITCHEN", msg.student.id, food_name(msg.student.food),
                  wait_time, prep_time);

        // Đẩy vào Q_WAITING (chờ bàn)
        msg.student.enter_queue_time = time(NULL);
        msgsnd(msgid_out, &msg, sizeof(Student), 0);
    }
    return 0;
}
