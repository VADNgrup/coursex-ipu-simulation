#include "sharedef.h"
#include <signal.h>

static volatile int running = 1;
void handle_sig(int sig) { running = 0; }

typedef struct {
    long   processed;
    double total_wait;
    double total_proc;
} QStats;

static void read_log_stats(QStats stats[3]) {
    memset(stats, 0, sizeof(QStats) * 3);
    FILE *f = fopen(LOG_FILE, "r");
    if (!f) return;

    char line[256];
    fgets(line, sizeof(line), f); // skip CSV header

    while (fgets(line, sizeof(line), f)) {
        long   ts;
        char   queue[32], food[16];
        int    sid;
        double wait, proc;
        if (sscanf(line, "%ld,%31[^,],%d,%15[^,],%lf,%lf",
                   &ts, queue, &sid, food, &wait, &proc) == 6) {
            int idx = -1;
            if      (strcmp(queue, "CASHING") == 0) idx = 0;
            else if (strcmp(queue, "KITCHEN") == 0) idx = 1;
            else if (strcmp(queue, "TABLE")   == 0) idx = 2;
            if (idx >= 0) {
                stats[idx].processed++;
                stats[idx].total_wait += wait;
                stats[idx].total_proc += proc;
            }
        }
    }
    fclose(f);
}

int main() {
    signal(SIGINT,  handle_sig);
    signal(SIGTERM, handle_sig);

    int mq_cashing = msgget(ftok(".", KEY_CASHING), 0666 | IPC_CREAT);
    int mq_food    = msgget(ftok(".", KEY_FOOD),    0666 | IPC_CREAT);
    int mq_waiting = msgget(ftok(".", KEY_WAITING), 0666 | IPC_CREAT);
    int mq_table   = msgget(ftok(".", KEY_TABLE),   0666 | IPC_CREAT);

    CafeteriaConfig *cfg = get_shared_config();

    printf("=== CAFETERIA DASHBOARD (Ctrl+C to stop) ===\n");

    while (running) {
        printf("\033[H\033[J"); // clear screen

        QStats stats[3];
        read_log_stats(stats);
        time_t now = time(NULL);

        long len_cashing = get_queue_count(mq_cashing);
        long len_food    = get_queue_count(mq_food);
        long len_waiting = get_queue_count(mq_waiting);
        long len_table   = get_queue_count(mq_table);
        int max_tables   = cfg ? cfg->num_tables : DEFAULT_NUM_TABLES;

        printf("===============================================================================\n");
        printf("                   CAFETERIA REAL-TIME MONITOR & DASHBOARD                     \n");
        printf("===============================================================================\n");
        if (cfg) {
            printf(" Config: Ramen:%d%% Kare:%d%% | Ramen:[%d-%ds] Kare:[%d-%ds] | Eat:[1-%ds] | Tables:%d\n",
                   cfg->ramen_ratio, 100 - cfg->ramen_ratio,
                   cfg->ramen_prep_min, cfg->ramen_prep_max,
                   cfg->kare_prep_min, cfg->kare_prep_max,
                   cfg->max_eating_time, cfg->num_tables);
            printf("-------------------------------------------------------------------------------\n");
        }
        printf("%-18s | %10s | %10s | %12s | %12s\n",
               "Queue Stage", "In Queue", "Completed", "Avg Wait (s)", "Avg Serv (s)");
        printf("-------------------------------------------------------------------------------\n");

        double wait_c = stats[0].processed > 0 ? stats[0].total_wait / stats[0].processed : 0.0;
        double proc_c = stats[0].processed > 0 ? stats[0].total_proc / stats[0].processed : 0.0;
        printf("%-18s | %10ld | %10ld | %12.2f | %12.2f\n",
               "1. CASHING (Pay)", len_cashing, stats[0].processed, wait_c, proc_c);

        double wait_k = stats[1].processed > 0 ? stats[1].total_wait / stats[1].processed : 0.0;
        double proc_k = stats[1].processed > 0 ? stats[1].total_proc / stats[1].processed : 0.0;
        printf("%-18s | %10ld | %10ld | %12.2f | %12.2f\n",
               "2. FOOD (Kitchen)", len_food, stats[1].processed, wait_k, proc_k);

        printf("%-18s | %10ld | %10s | %12s | %12s\n",
               "3. WAITING (Table)", len_waiting, "-", "-", "-");

        double wait_t = stats[2].processed > 0 ? stats[2].total_wait / stats[2].processed : 0.0;
        double proc_t = stats[2].processed > 0 ? stats[2].total_proc / stats[2].processed : 0.0;
        char table_occupancy[32];
        snprintf(table_occupancy, sizeof(table_occupancy), "%ld/%d tables", len_table, max_tables);
        printf("%-18s | %10s | %10ld | %12.2f | %12.2f\n",
               "4. TABLE (Eating)", table_occupancy, stats[2].processed, wait_t, proc_t);

        printf("===============================================================================\n");

        // Ghi vào DASHBOARD_CSV để phục vụ vẽ biểu đồ
        FILE *csv = fopen(DASHBOARD_CSV, "a");
        if (csv) {
            fprintf(csv, "%ld,CASHING,%ld,%ld,%.2f,%.2f\n", now, len_cashing, stats[0].processed, wait_c, proc_c);
            fprintf(csv, "%ld,FOOD,%ld,%ld,%.2f,%.2f\n", now, len_food, stats[1].processed, wait_k, proc_k);
            fprintf(csv, "%ld,WAITING,%ld,0,0.0,0.0\n", now, len_waiting);
            fprintf(csv, "%ld,TABLE,%ld,%ld,%.2f,%.2f\n", now, len_table, stats[2].processed, wait_t, proc_t);
            fclose(csv);
        }

        sleep(1);
    }

    printf("Dashboard stopped.\n");
    return 0;
}
