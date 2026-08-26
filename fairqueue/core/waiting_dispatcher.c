#include "sharedef.h"
#include <signal.h>

static volatile int running = 1;
void handle_sig(int sig) { running = 0; }

// Dispatcher:
// - Kiểm tra user trong Q_WAITING
// - Nếu mảng bàn ăn trong Shared Memory còn slot trống (< num_tables) -> gán user vào slot bàn
// - Nếu đầy -> user tiếp tục chờ trong Q_WAITING
int main() {
    signal(SIGINT,  handle_sig);
    signal(SIGTERM, handle_sig);

    int msgid_waiting = msgget(ftok(".", KEY_WAITING), 0666 | IPC_CREAT);
    if (msgid_waiting < 0) { perror("MSGGET Error"); return 1; }

    CafeteriaConfig *cfg = get_shared_config();
    if (!cfg) {
        fprintf(stderr, "[ERROR] Cannot access Shared Config/Tables!\n");
        return 1;
    }

    srand(time(NULL) + 4);

    printf("=== WAITING QUEUE DISPATCHER STARTED ===\n");
    printf("Monitoring Q_WAITING and dispatching to Shared Memory Tables when slots are free...\n");

    while (running) {
        int max_tables = cfg->num_tables;
        if (max_tables < 1) max_tables = 1;
        if (max_tables > MAX_TABLE_SLOTS) max_tables = MAX_TABLE_SLOTS;

        // Tìm các slot bàn trống và xếp sinh viên từ Q_WAITING vào
        for (int i = 0; i < max_tables; i++) {
            if (!cfg->tables[i].occupied) {
                struct mesg_buffer msg;
                // Thử lấy 1 student đang chờ trong waiting queue
                if (msgrcv(msgid_waiting, &msg, sizeof(Student), 0, IPC_NOWAIT) != -1) {
                    double wait_time = difftime(time(NULL), msg.student.enter_queue_time);
                    msg.student.wait_time_table = wait_time;

                    int eat_time = msg.student.eating_time;
                    if (eat_time <= 0) {
                        int max_eat = cfg->max_eating_time;
                        if (max_eat < 1) max_eat = 1;
                        eat_time = 1 + rand() % max_eat;
                    }

                    // Gán vào bàn i
                    cfg->tables[i].student = msg.student;
                    cfg->tables[i].remaining_time = eat_time;
                    cfg->tables[i].total_eat_time = eat_time;
                    cfg->tables[i].occupied = 1;

                    int occupied_count = count_occupied_tables(cfg);
                    printf("[WaitingDispatcher] Student %d (%s) sits at Table #%d (%d/%d tables in use) | Eat: %ds | Waited: %.0fs\n",
                           msg.student.id, food_name(msg.student.food), i + 1, occupied_count, max_tables, eat_time, wait_time);
                } else {
                    // Không còn ai trong Q_WAITING tại thời điểm này
                    break;
                }
            }
        }

        usleep(100000); // 100ms
    }

    printf("Waiting dispatcher stopped.\n");
    return 0;
}
