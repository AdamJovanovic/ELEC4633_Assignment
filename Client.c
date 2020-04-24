//ELEC4633 Lab02 : Client program to assign the setpoint values

#include <stdio.h>
#include <sys/msg.h>
#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>

struct mymsgbuf {
  long mtype;
  int setpoint;
};


struct .................;
//int motorWriteBuffer[WRITELENGTH];

int main(void)
{
    //messege passing variables
    int status, i;
    int queue;
    key_t msgkey;
    
    // generate a key to obtain a queue id
    msgkey = ftok(".",'m');

  // obtain a queue identifier
    queue = msgget(msgkey, IPC_CREAT | 0660);
    
    // set the message type to 1 - can be any positive number
    msg01.mtype = 1;

    int setpoint = atoi(argv[1]);
    
    msg01............. = 1000;
    
    while(1)
    {
    scanf("%d", &(msg01.setpoint))
    status = msgsnd(........, ....., sizeof(...........), 0);
    sleep(1)
    }
	
    return 0;
}
