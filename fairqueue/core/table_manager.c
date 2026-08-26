#include "sharedef.h"

// Table Manager:
// - Lấy 1 student từ Q_TABLE
// - Random thời gian ăn trong khoảng (1 -> max_eating_time)
// - Sleep
// - Xóa/hoàn tất (rời khỏi căng tin)
int main() {
    int msgid_table = msgget(ftok(".", KEY_TABLE), 0666 | IPC_CREAT);
    if (msgid_table < 0) { perror("MSGGET Error"); return 1; }

    CafeteriaConfig *cfg = get_shared_config();
    srand(time(NULL) + 3);

    printf("=== TABLE MANAGER (EATING WORKER) STARTED ===\n");
    printf("Listening on Q_TABLE...\n");

    while (1) {
        struct mesg_buffer msg;
        // Block chờ student trong Q_TABLE
        if (msgrcv(msgid_table, &msg, sizeof(Student), 0, 0) == -1) continue;

        double wait_time = difftime(time(NULL), msg.student.enter_queue_time);

        int max_eat = cfg ? cfg->max_eating_time : DEFAULT_MAX_EATING_TIME;
        if (max_eat < 1) max_eat = 1;

        // Thời gian dùng 1 bàn random trong khoảng 1 -> max_eating_time
        int eat_time = 1 + rand() % max_eat;

        printf("[Table] Student %d sits at table | %s | Wait: %.0fs | Eating: %ds (max=%ds)\n",
               msg.student.id, food_name(msg.student.food), wait_time, eat_time, max_eat);

        sleep(eat_time);

        printf("[Table] Student %d finished eating & left the cafeteria. Table freed!\n",
               msg.student.id);

        write_log("TABLE", msg.student.id, food_name(msg.student.food),
                  wait_time, eat_time);
    }

    return 0;
}
