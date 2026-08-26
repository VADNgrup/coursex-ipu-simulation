#ifndef SHARED_DEF_H
#define SHARED_DEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <pthread.h>

#define SHM_KEY 0x8888
#define MAX_QUEUE 200
#define HEARTBEAT_TIMEOUT 3 // Quá 3s không có heartbeat -> Coi như rạp ĐÃ ĐÓNG

// Customer
typedef struct {
    int id;
    time_t enter_fair_time;
    time_t enter_queue_time;
    int visited[4]; 
} Customer;

//  (Circular Queue)
typedef struct {
    Customer data[MAX_QUEUE];
    int head;
    int tail;
    int count;
} Queue;

//  (Stall)
typedef struct {
    char name[32];
    int is_optional;      // 0: Unique (Ticket), 1: Optional (Food)
    int status;           // 1: ON, 0: OFF
    time_t last_heartbeat;

    Queue in_queue;
    Queue out_queue;

    int total_processed;
    double total_process_time;
    double total_wait_time;
} Stall;
// ================================
//  Shared Memory 
// ================================
typedef struct {
    Stall stalls[4]; // 0: Ticket Check, 1: Chicken, 2: Takoyaki, 3: Yakiniku
    int fair_people_count;
    pthread_mutex_t lock;
} FairData;

// ================================
//  Queue function
// ================================
static inline void init_queue(Queue *q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

static inline int push_queue(Queue *q, Customer c) {
    if (q->count >= MAX_QUEUE) return 0;
    q->data[q->tail] = c;
    q->tail = (q->tail + 1) % MAX_QUEUE;
    q->count++;
    return 1;
}

static inline int pop_queue(Queue *q, Customer *c) {
    if (q->count == 0) return 0;
    *c = q->data[q->head];
    q->head = (q->head + 1) % MAX_QUEUE;
    q->count--;
    return 1;
}

#endif