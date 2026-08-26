#include "sharedef.h"

static void init_q(int key_val, const char *name) {
    key_t key = ftok(".", key_val);
    int old = msgget(key, 0666);
    if (old != -1) msgctl(old, IPC_RMID, NULL);
    int id = msgget(key, 0666 | IPC_CREAT);
    if (id < 0) printf("  [ERROR] Cannot create %s\n", name);
    else        printf("  %-32s OK (msgid=%d)\n", name, id);
}

int main() {
    printf("=== Initializing Cafeteria Message Queues & Config ===\n");
    init_q(KEY_CASHING, "Q_CASHING (1. Ordering/Payment)");
    init_q(KEY_FOOD,    "Q_FOOD    (2. Cooking/Kitchen)");
    init_q(KEY_WAITING, "Q_WAITING (3. Waiting for Table)");

    // Clean up legacy KEY_TABLE queue if exists
    key_t old_table_key = ftok(".", KEY_TABLE);
    int old_table_q = msgget(old_table_key, 0666);
    if (old_table_q != -1) {
        msgctl(old_table_q, IPC_RMID, NULL);
    }

    // Reset and Initialize Shared Configuration & Tables
    key_t shm_key = ftok(".", KEY_CONFIG_SHM);
    int old_shm = shmget(shm_key, 0, 0666);
    if (old_shm != -1) {
        shmctl(old_shm, IPC_RMID, NULL);
    }

    CafeteriaConfig *cfg = get_shared_config();
    if (cfg) {
        memset(cfg, 0, sizeof(CafeteriaConfig));
        cfg->ramen_ratio       = DEFAULT_RAMEN_RATIO;
        cfg->ramen_prep_min    = DEFAULT_RAMEN_PREP_MIN;
        cfg->ramen_prep_max    = DEFAULT_RAMEN_PREP_MAX;
        cfg->kare_prep_min     = DEFAULT_KARE_PREP_MIN;
        cfg->kare_prep_max     = DEFAULT_KARE_PREP_MAX;
        cfg->max_eating_time   = DEFAULT_MAX_EATING_TIME;
        cfg->num_tables        = DEFAULT_NUM_TABLES;
        printf("  Shared Config & Table Array initialized (Capacity: %d tables, Active: %d tables).\n",
               MAX_TABLE_SLOTS, cfg->num_tables);
    } else {
        printf("  [ERROR] Failed to initialize Shared Config.\n");
    }

    // Init log file with CSV header
    FILE *f = fopen(LOG_FILE, "w");
    if (f) {
        fprintf(f, "timestamp,queue,student_id,food,wait_time,proc_time\n");
        fclose(f);
        printf("  Log file created: %s\n", LOG_FILE);
    }

    FILE *d = fopen(DASHBOARD_CSV, "w");
    if (d) {
        fprintf(d, "timestamp,queue,current_length,total_processed,avg_wait_time,avg_proc_time\n");
        fclose(d);
        printf("  Dashboard CSV  : %s\n", DASHBOARD_CSV);
    }

    printf("Done!\n");
    return 0;
}
