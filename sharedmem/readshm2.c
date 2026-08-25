#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define SHMSIZ 1024

int main()
{ 
    char c, tmp;
    int shmid;
    key_t key;
    char *shm;

    key = 1234;

    if((shmid = shmget(key, SHMSIZ, 0666)) < 0)
    { 
        perror("shmget error"); 
        return 1; 
    }

    if((shm = shmat(shmid, NULL, 0)) == (char *) -1)
    { 
        perror("shmat error"); 
        return 1; 
    }

    printf("shmid=%x\n", shmid);  
    printf("shm pointer = %p     hit any key to start\n", (void *)shm);

    while(1)
    {  
        c = *(shm+1);
        printf("shm char = %c\n", c);
        fflush(stdout); // Đẩy dữ liệu ra màn hình ngay lập tức
        sleep(5);      // Tạm dừng 1 giây
    }

    return 0;
}