#include "sharedef.h"

int main() {
    key_t key = ftok(".", KEY_CASHING);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid < 0) { perror("MSGGET Error"); return 1; }

    CafeteriaConfig *cfg = get_shared_config();

    int total_limit, interval, max_batch;
    printf("=== STUDENT GENERATOR ===\n");
    printf("Total students limit to generate: ");
    if (scanf("%d", &total_limit) != 1 || total_limit <= 0) total_limit = 50;

    printf("Max students per batch: ");
    if (scanf("%d", &max_batch) != 1 || max_batch <= 0) max_batch = 3;

    printf("Interval between batches (seconds): ");
    if (scanf("%d", &interval) != 1 || interval <= 0) interval = 2;

    int id_counter = 1;
    srand(time(NULL));

    printf("Starting generation: limit=%d students, batch<=%d, interval=%ds...\n",
           total_limit, max_batch, interval);

    while (id_counter <= total_limit) {
        sleep(interval);

        // int count = 1 + rand() % max_batch;
        int count = max_batch;
        for (int i = 0; i < count && id_counter <= total_limit; i++) {
            struct mesg_buffer msg;
            msg.mesg_type          = 1;
            msg.student.id         = id_counter++;
            msg.student.arrive_time      = time(NULL);
            msg.student.enter_queue_time = time(NULL);

            // Quyết định món dựa trên ramen_ratio được cấu hình
            int ratio = cfg ? cfg->ramen_ratio : DEFAULT_RAMEN_RATIO;
            if (rand() % 100 < ratio) {
                msg.student.food = FOOD_RAMEN;
            } else {
                msg.student.food = FOOD_KARE;
            }

            if (msgsnd(msgid, &msg, sizeof(Student), 0) == 0) {
                printf("[Generator] Student %d (%s) -> Q_CASHING [%d/%d]\n",
                       msg.student.id, food_name(msg.student.food),
                       msg.student.id, total_limit);
            }
        }
    }

    printf("[Generator] Finished generating all %d students.\n", total_limit);
    return 0;
}
