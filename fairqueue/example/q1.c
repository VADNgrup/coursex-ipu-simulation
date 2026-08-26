#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
//#include <time.h>

struct mesg_buffer {
long mesg_type;
int mesg_text[10];
} message;

struct msqid_ds buf;

int main()
{key_t key;
int msgid, r;
int n_msg;
char c;

key = ftok(".", 65);
msgid = msgget(key, 0666 | IPC_CREAT);
printf("msgid=%d\n", msgid);

n_msg=1;
while(1)
{

msgctl(msgid, IPC_STAT, &buf);
printf("#of msg:   %ld\n", buf.msg_qnum);
if( buf.msg_qnum >= 10 )
{
	sleep(2);
	printf("Q0 is full, ...\n");
	continue;
}


sleep(1);
message.mesg_type = 1;
message.mesg_text[0]=n_msg++;
r=msgsnd(msgid, &message, sizeof(message), 0);
printf("r=%d Data sent to Q0 is : %d \n",
		r,message.mesg_text[0]);
//printf("err=%d\n", errno);
}


msgctl(msgid, IPC_STAT, &buf);
printf("#of msg:   %ld\n", buf.msg_qnum);
//msgctl(msgid, IPC_RMID, NULL);
return 0;
}
