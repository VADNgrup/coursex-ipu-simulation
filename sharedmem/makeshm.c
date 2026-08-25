#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define SHMSIZ 1024

main()
{ char c, tmp;
  int shmid;
  key_t key;

  key = 1234;

  if( (shmid = shmget(key, SHMSIZ, IPC_CREAT | 0666) ) < 0)
  { perror("shmget error"); return 1; }
}
