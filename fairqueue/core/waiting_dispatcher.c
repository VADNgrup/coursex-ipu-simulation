#include "sharedef.h"
#include <signal.h>

static volatile int running = 1;
void handle_sig(int sig) { running = 0; }

// Dispatcher:
// - Kiểm tra user trong Q_WAITING
// - Nếu Q_TABLE còn trống slot (< num_tables) -> chuyển user vào Q_TABLE
// - Nếu không -> user tiếp tục chờ trong Q_WAITING
int main() {
    signal(SIGINT,  handle_sig);
    signal(SIGTERM, handle_sig);

    int msgid_waiting = msgget(ftok(".", KEY_WAITING), 0666 | IPC_CREAT);
    int msgid_table   = msgget(ftok(".", KEY_TABLE),   0666 | IPC_CREAT);
    if (msgid_waiting < 0 || msgid_table < 0) { perror("MSGGET Error"); return 1; }

    CafeteriaConfig *cfg = get_shared_config();

    printf("=== WAITING QUEUE DISPATCHER STARTED ===\n");
    printf("Monitoring Q_WAITING and dispatching to Q_TABLE when slots are free...\n");

    while (running) {
        int max_tables = cfg ? cfg->num_tables : DEFAULT_NUM_TABLES;
        if (max_tables < 1) max_tables = 1;

        long current_eating = get_queue_count(msgid_table);

        // Kiểm tra xem table queue còn chỗ không
        if (current_eating < max_tables) {
            struct mesg_buffer msg;
            // Lấy 1 student đang chờ trong waiting queue
            if (msgrcv(msgid_waiting, &msg, sizeof(Student), 0, IPC_NOWAIT) != -1) {
                double wait_time = difftime(time(NULL), msg.student.enter_queue_time);

                printf("[WaitingDispatcher] Table free (%ld/%d in use). Student %d -> Q_TABLE (Waited: %.0fs)\n",
                       current_eating + 1, max_tables, msg.student.id, wait_time);

                // Chuyển sang Q_TABLE
                // msg.student.enter_queue_time = time(NULL);
                msgsnd(msgid_table, &msg, sizeof(Student), 0);
            }
        }

        usleep(100000); // 100ms
    }

    printf("Waiting dispatcher stopped.\n");
    return 0;
}
