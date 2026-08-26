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
//  MQ Keys (4 Message Queues)
// ============================================================
#define KEY_CASHING  65   // 1. Queue chờ tính tiền (Cashier)
#define KEY_FOOD     66   // 2. Queue chờ làm món (Kitchen)
#define KEY_WAITING  67   // 3. Queue chờ bàn (Waiting for Table)
#define KEY_TABLE    68   // 4. Queue bàn ăn thực tế (Table Queue, giới hạn num_tables)

// Shared Memory Key for Dynamic Hyperparameters
#define KEY_CONFIG_SHM 77

// ============================================================
//  Food Configuration Structure (Dynamic via SHM)
// ============================================================
typedef enum { FOOD_RAMEN = 1, FOOD_KARE = 2 } FoodType;

typedef struct {
    int ramen_ratio;         // Tỉ lệ % chọn Ramen (0 - 100), Kare = 100 - ramen_ratio
    int ramen_prep_min;
    int ramen_prep_max;
    int kare_prep_min;
    int kare_prep_max;
    int max_eating_time;     // Thời gian ăn tối đa (1 -> max_eating_time)
    int num_tables;          // Giới hạn số bàn (số slot trong KEY_TABLE)
} CafeteriaConfig;

// Default initial config values
#define DEFAULT_RAMEN_RATIO      50  // 50% Ramen, 50% Kare
#define DEFAULT_RAMEN_PREP_MIN   3
#define DEFAULT_RAMEN_PREP_MAX   6
#define DEFAULT_KARE_PREP_MIN    5
#define DEFAULT_KARE_PREP_MAX    9
#define DEFAULT_MAX_EATING_TIME  15
#define DEFAULT_NUM_TABLES       5

// ============================================================
//  Helper: Get attached pointer to Shared Config
// ============================================================
static inline CafeteriaConfig* get_shared_config() {
    int shmid = shmget(ftok(".", KEY_CONFIG_SHM), sizeof(CafeteriaConfig), 0666 | IPC_CREAT);
    if (shmid < 0) return NULL;
    CafeteriaConfig *cfg = (CafeteriaConfig*)shmat(shmid, NULL, 0);
    if (cfg == (void*)-1) return NULL;
    return cfg;
}

// ============================================================
//  Student Structure
// ============================================================
typedef struct {
    int      id;
    FoodType food;              // được chọn ngay lúc generator tạo ra
    time_t   arrive_time;       // thời điểm vào hệ thống
    time_t   enter_queue_time;  // thời điểm vào từng queue
} Student;

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
#define LOG_FILE           "/tmp/cafeteria_log.csv"
#define DASHBOARD_CSV      "/tmp/cafeteria_dashboard.csv"

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
