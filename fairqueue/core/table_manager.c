#include "sharedef.h"
#include <signal.h>

static volatile int running = 1;
void handle_sig(int sig) { running = 0; }

// Table Manager:
// - Quản lý mảng bàn ăn trong Shared Memory
// - Trong mỗi vòng lặp (mỗi giây), đếm ngược eating_time của từng student đang ngồi ăn
// - Khi eating_time == 0:
//     + Ghi log hoàn thành ("TABLE")
//     + Thông báo sinh viên rời khỏi căng tin & bàn được giải phóng
//     + Reset slot bàn (occupied = 0) để waiting_dispatcher có thể xếp người tiếp theo vào
int main() {
    signal(SIGINT,  handle_sig);
    signal(SIGTERM, handle_sig);

    CafeteriaConfig *cfg = get_shared_config();
    if (!cfg) {
        fprintf(stderr, "[ERROR] Cannot access Shared Config/Tables!\n");
        return 1;
    }

    printf("=== TABLE MANAGER (SHARED MEMORY WORKER) STARTED ===\n");
    printf("Monitoring and counting down eating time for all active tables in parallel...\n");

    while (running) {
        sleep(1);

        int active_count = 0;
        for (int i = 0; i < MAX_TABLE_SLOTS; i++) {
            if (cfg->tables[i].occupied) {
                cfg->tables[i].remaining_time--;

                if (cfg->tables[i].remaining_time <= 0) {
                    // Sinh viên đã ăn xong
                    Student s = cfg->tables[i].student;
                    int eat_time = cfg->tables[i].total_eat_time;
                    double wait_time = s.wait_time_table;

                    printf("[Table] Student %d finished eating at Table #%d & left cafeteria. Table freed! (Ate: %ds)\n",
                           s.id, i + 1, eat_time);

                    write_log("TABLE", s.id, food_name(s.food), wait_time, eat_time);

                    // Giải phóng slot bàn
                    cfg->tables[i].occupied = 0;
                    cfg->tables[i].remaining_time = 0;
                    cfg->tables[i].total_eat_time = 0;
                    memset(&cfg->tables[i].student, 0, sizeof(Student));
                } else {
                    active_count++;
                }
            }
        }
    }

    printf("Table manager stopped.\n");
    return 0;
}
