#ifndef SHARED_DEF_H
#define SHARED_DEF_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SHM_KEY 1234
#define SHMSIZ sizeof(MicrowaveData)

typedef enum {
    POWER_OFF,
    IDLE,
    DOOR_OPEN,
    COOKING,
    PAUSED,
    FINISHED
} SystemState;

typedef struct {
    // Kênh nhận lệnh từ người dùng (Controller -> Engine)
    int cmd; // 0: None, 1: Elec Toggle, 2: Door Toggle, 3: Add 30s, 4: Switch Power, 5: Start, 6: Stop

    // Các biến trạng thái của Lò vi sóng
    int elec;     // 1: ON, 0: OFF
    int door;     // 1: OPEN, 0: CLOSE
    int timer;    // 0 -> 600s
    int power;    // 500 hoặc 700
    int emitter;  // 1: ON, 0: OFF
    int bell;     // 1: RING, 0: NO
    int light;    // 1: ON, 0: OFF
    
    SystemState state;
} MicrowaveData;

#endif