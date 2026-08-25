#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define SHMSIZ 1024

main()
{ char c[2], tmp;
  int shmid;
  key_t key;
  char *shm;

  key = 1234;

  if( (shmid = shmget(key, SHMSIZ, 0666) ) < 0)
  { perror("shmget error"); return 1; }

  if( (shm = shmat(shmid,NULL, 0)) == (char *) -1 )
  { perror("shmat error"); return 1; }
 
  printf("shmid=%x\n", shmid);  
  // printf("shm pointer =%x\n", shm);  
  printf("shm pointer = %p\n", (void *)shm);

while(1)
{  printf("enter one char ->  ");
   scanf("%s", c);
   *shm = c[0];

   printf("enter one char ->  ");
   scanf("%s", c);
   *(shm+1) = c[0];

}

}
