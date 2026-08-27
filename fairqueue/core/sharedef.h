#ifndef SHAREDEF_H
#define SHAREDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

// ============================================================
//  MQ Keys (3 Message Queues)
// ============================================================
#define KEY_CASHING  65   // 1. Queue chờ tính tiền (Cashier)
#define KEY_FOOD     66   // 2. Queue chờ làm món (Kitchen)
#define KEY_WAITING  67   // 3. Queue chờ bàn (Waiting for Table)
#define KEY_TABLE    68   // (Legacy key)

// Shared Memory Key for Dynamic Hyperparameters & Table Array
#define KEY_CONFIG_SHM 77

#define MAX_TABLE_SLOTS 100

// ============================================================
//  Food Configuration Structure (Dynamic via SHM)
// ============================================================
typedef enum { FOOD_RAMEN = 1, FOOD_KARE = 2 } FoodType;

// ============================================================
//  Student Structure
// ============================================================
typedef struct {
    int      id;
    FoodType food;              // Món ăn được chọn (Ramen / Kare)
    time_t   arrive_time;       // Thời điểm vào hệ thống
    time_t   enter_queue_time;  // Thời điểm vào từng queue
    int      eating_time;       // Thời gian ăn (giây)
    double   wait_time_table;   // Thời gian chờ ở Q_WAITING trước khi có bàn
} Student;

// ============================================================
//  Table Slot Structure (in Shared Memory)
// ============================================================
typedef struct {
    int     occupied;          // 0: Bàn trống, 1: Đang có sinh viên ngồi ăn
    Student student;           // Thông tin sinh viên đang ngồi tại bàn
    int     remaining_time;    // Đếm ngược thời gian ăn còn lại (giây)
    int     total_eat_time;    // Tổng thời gian ăn dự kiến (giây)
} TableSlot;

typedef struct {
    int ramen_ratio;         // Tỉ lệ % chọn Ramen (0 - 100), Kare = 100 - ramen_ratio
    int ramen_prep_min;
    int ramen_prep_max;
    int kare_prep_min;
    int kare_prep_max;
    int max_eating_time;     // Thời gian ăn tối đa (1 -> max_eating_time)
    int num_tables;          // Giới hạn số bàn đang sử dụng (<= MAX_TABLE_SLOTS)

    // Mảng bàn ăn mô phỏng ăn song song trong Shared Memory
    TableSlot tables[MAX_TABLE_SLOTS];
} CafeteriaConfig;

// Default initial config values
#define DEFAULT_RAMEN_RATIO      50  // 50% Ramen, 50% Kare
#define DEFAULT_RAMEN_PREP_MIN   15
#define DEFAULT_RAMEN_PREP_MAX   25
#define DEFAULT_KARE_PREP_MIN    20
#define DEFAULT_KARE_PREP_MAX    30
#define DEFAULT_MAX_EATING_TIME  900 // 15 minutes
#define DEFAULT_NUM_TABLES       50

// ============================================================
//  Helper: Get attached pointer to Shared Config & Tables
// ============================================================
static inline CafeteriaConfig* get_shared_config() {
    int shmid = shmget(ftok(".", KEY_CONFIG_SHM), sizeof(CafeteriaConfig), 0666 | IPC_CREAT);
    if (shmid < 0) return NULL;
    CafeteriaConfig *cfg = (CafeteriaConfig*)shmat(shmid, NULL, 0);
    if (cfg == (void*)-1) return NULL;
    return cfg;
}

// ============================================================
//  Helper: Count active / occupied tables
// ============================================================
static inline int count_occupied_tables(CafeteriaConfig *cfg) {
    if (!cfg) return 0;
    int count = 0;
    int max_t = cfg->num_tables;
    if (max_t > MAX_TABLE_SLOTS) max_t = MAX_TABLE_SLOTS;
    if (max_t < 1) max_t = 1;
    for (int i = 0; i < max_t; i++) {
        if (cfg->tables[i].occupied) {
            count++;
        }
    }
    return count;
}

// ============================================================
//  Message Buffer
// ============================================================
struct mesg_buffer {
    long    mesg_type;   // always 1
    Student student;
};

// ============================================================
//  Monitoring Log File
// ============================================================
#define LOG_FILE           "./.tmp/cafeteria_log.csv"
#define DASHBOARD_CSV      "./.tmp/cafeteria_dashboard.csv"

// ============================================================
//  Helper: get queue length
// ============================================================
static inline long get_queue_count(int msgid) {
    struct msqid_ds buf;
    if (msgctl(msgid, IPC_STAT, &buf) == -1) return -1;
    return buf.msg_qnum;
}

// ============================================================
//  Helper: get food name string
// ============================================================
static inline const char* food_name(FoodType f) {
    if (f == FOOD_RAMEN) return "RAMEN";
    if (f == FOOD_KARE)  return "KARE";
    return "NONE";
}

// ============================================================
//  Helper: append one monitoring record to log file
// ============================================================
static inline void write_log(const char *queue, int student_id,
                              const char *food,
                              double wait_time, double proc_time) {
    FILE *f = fopen(LOG_FILE, "a");
    if (!f) return;
    fprintf(f, "%ld,%s,%d,%s,%.2f,%.2f\n",
            (long)time(NULL), queue, student_id, food, wait_time, proc_time);
    fclose(f);
}

#endif
