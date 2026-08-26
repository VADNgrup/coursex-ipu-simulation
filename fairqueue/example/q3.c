#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
struct mesg_buffer {
long mesg_type;
int mesg_text[10];
} message;

struct msqid_ds buf;

int main()
{key_t key;
int msgid1;
int quit;

key = ftok(".", 66);
msgid1 = msgget(key, 0666 | IPC_CREAT);
printf(" msgid1=%d\n",  msgid1);

while (1)
{
msgctl(msgid1, IPC_STAT, &buf);
printf("# of msgs, %ld\n", buf.msg_qnum);

msgrcv(msgid1, &message, sizeof(message), 0, 0);
message.mesg_text[0]++;
printf("(q3) Data Received from Q1 is : %d \n",message.mesg_text[0]);

//msgctl(msgid, IPC_RMID, NULL);
}

return 0;
}
