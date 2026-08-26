#include "sharedef.h"

// Cashier:
// - Nhận student từ Q_CASHING
// - Sleep thời gian thanh toán ngẫu nhiên (1-3s)
// - Đẩy sang Q_FOOD
int main() {
    int msgid_in  = msgget(ftok(".", KEY_CASHING), 0666 | IPC_CREAT);
    int msgid_out = msgget(ftok(".", KEY_FOOD),    0666 | IPC_CREAT);
    if (msgid_in < 0 || msgid_out < 0) { perror("MSGGET Error"); return 1; }

    srand(time(NULL) + 1);
    printf("=== CASHIER STARTED ===\n");
    printf("Waiting for students in Q_CASHING...\n");

    while (1) {
        struct mesg_buffer msg;
        if (msgrcv(msgid_in, &msg, sizeof(Student), 0, 0) == -1) continue;

        double wait_time = difftime(time(NULL), msg.student.enter_queue_time);

        // Sleep ngẫu nhiên thời gian thanh toán
        int proc_time = 1 + rand() % 3;
        sleep(proc_time);

        printf("[Cashier] Student %d | %s | Wait: %.0fs | Pay: %ds\n",
               msg.student.id, food_name(msg.student.food), wait_time, proc_time);

        write_log("CASHING", msg.student.id, food_name(msg.student.food),
                  wait_time, proc_time);

        // Đẩy sang Q_FOOD
        msg.student.enter_queue_time = time(NULL);
        msgsnd(msgid_out, &msg, sizeof(Student), 0);
    }
    return 0;
}
