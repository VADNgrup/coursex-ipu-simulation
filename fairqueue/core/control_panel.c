#include "sharedef.h"

void print_menu(CafeteriaConfig *cfg) {
    printf("\n=======================================================\n");
    printf("         CAFETERIA DYNAMIC CONTROL PANEL               \n");
    printf("=======================================================\n");
    printf(" 1. Food Choice Ratio     : %d%% Ramen / %d%% Kare\n", cfg->ramen_ratio, 100 - cfg->ramen_ratio);
    printf(" 2. Ramen Prep Time Range : [%d - %d] seconds\n", cfg->ramen_prep_min, cfg->ramen_prep_max);
    printf(" 3. Kare Prep Time Range  : [%d - %d] seconds\n", cfg->kare_prep_min, cfg->kare_prep_max);
    printf(" 4. Max Student Eat Time  : [1 - %d] seconds\n", cfg->max_eating_time);
    printf(" 5. Number of Tables      : %d tables\n", cfg->num_tables);
    printf(" 6. Reset to Default Values\n");
    printf(" 0. Exit Control Panel\n");
    printf("=======================================================\n");
    printf("Enter choice (0-6): ");
}

int main() {
    CafeteriaConfig *cfg = get_shared_config();
    if (!cfg) {
        printf("[ERROR] Cannot access Shared Config. Make sure ./init_mq.sh has been run!\n");
        return 1;
    }

    int choice;
    while (1) {
        print_menu(cfg);
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

        if (choice == 0) {
            printf("Exiting Control Panel.\n");
            break;
        }

        int min_val, max_val, num_val;
        switch (choice) {
            case 1:
                printf("Enter new Ramen selection ratio %% (0-100): ");
                scanf("%d", &num_val);
                if (num_val >= 0 && num_val <= 100) {
                    cfg->ramen_ratio = num_val;
                    printf(">> Updated Food Ratio: %d%% Ramen, %d%% Kare!\n", num_val, 100 - num_val);
                } else {
                    printf(">> Invalid percentage (0-100)!\n");
                }
                break;

            case 2:
                printf("Enter new Ramen Prep Min (sec): ");
                scanf("%d", &min_val);
                printf("Enter new Ramen Prep Max (sec): ");
                scanf("%d", &max_val);
                if (min_val > 0 && max_val >= min_val) {
                    cfg->ramen_prep_min = min_val;
                    cfg->ramen_prep_max = max_val;
                    printf(">> Updated Ramen prep time to [%d - %d]s!\n", min_val, max_val);
                } else {
                    printf(">> Invalid range!\n");
                }
                break;

            case 3:
                printf("Enter new Kare Prep Min (sec): ");
                scanf("%d", &min_val);
                printf("Enter new Kare Prep Max (sec): ");
                scanf("%d", &max_val);
                if (min_val > 0 && max_val >= min_val) {
                    cfg->kare_prep_min = min_val;
                    cfg->kare_prep_max = max_val;
                    printf(">> Updated Kare prep time to [%d - %d]s!\n", min_val, max_val);
                } else {
                    printf(">> Invalid range!\n");
                }
                break;

            case 4:
                printf("Enter new Max Eating Time (sec, random 1 -> max): ");
                scanf("%d", &max_val);
                if (max_val >= 1) {
                    cfg->max_eating_time = max_val;
                    printf(">> Updated Max Eating Time to 1 -> %ds!\n", max_val);
                } else {
                    printf(">> Invalid max eating time!\n");
                }
                break;

            case 5:
                printf("Enter new Number of Tables (1-100): ");
                scanf("%d", &num_val);
                if (num_val >= 1 && num_val <= 100) {
                    cfg->num_tables = num_val;
                    printf(">> Updated Number of Tables to %d!\n", num_val);
                } else {
                    printf(">> Invalid number of tables (1-100)!\n");
                }
                break;

            case 6:
                cfg->ramen_ratio       = DEFAULT_RAMEN_RATIO;
                cfg->ramen_prep_min    = DEFAULT_RAMEN_PREP_MIN;
                cfg->ramen_prep_max    = DEFAULT_RAMEN_PREP_MAX;
                cfg->kare_prep_min     = DEFAULT_KARE_PREP_MIN;
                cfg->kare_prep_max     = DEFAULT_KARE_PREP_MAX;
                cfg->max_eating_time   = DEFAULT_MAX_EATING_TIME;
                cfg->num_tables        = DEFAULT_NUM_TABLES;
                printf(">> Reset all hyperparameters to default values!\n");
                break;

            default:
                printf(">> Invalid selection. Please choose 0-6.\n");
                break;
        }
    }
    return 0;
}
