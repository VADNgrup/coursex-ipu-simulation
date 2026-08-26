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
int msgid;
int msgid1;
int r, quit;


key = ftok(".", 65);
msgid = msgget(key, 0666 | IPC_CREAT);
key = ftok(".", 66);
msgid1 = msgget(key, 0666 | IPC_CREAT);
printf("MSGID=%d msgid1=%d\n", msgid, msgid1);

while (1)
{
msgctl(msgid, IPC_STAT, &buf);
printf("# of msgs, %ld\n", buf.msg_qnum);

msgrcv(msgid, &message, sizeof(message), 0, 0);
printf("Data Received from Q0 is : %d \n",message.mesg_text[0]);

sleep(3); // processing delay
message.mesg_type = 1;
message.mesg_text[0]++;
r=msgsnd(msgid1, &message, sizeof(message), 0);
printf("r=%d Data sent to Q1 is : %d \n",
                r,message.mesg_text[0]);

//msgctl(msgid, IPC_RMID, NULL);
}

return 0;
}
