//ELEC4633 Lab02 : Client program to assign the setpoint values

#include <stdio.h>
#include <sys/msg.h>
#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>

#define wait_1_second() sleep(1)

typedef struct setpointmessage {
    long mtype;
    int setpoint;
} SetpointMessage;

int main(int argc, char *argv[])
{
    SetpointMessage mySetpointSend;

    //messege passing variables
    int status, i;
    int queue;
    key_t msgkey;
    
    // generate a key to obtain a queue id
    msgkey = ftok(".",'m');

    // obtain a queue identifier (To match identiier in the Server.c program)
    queue = msgget(msgkey, IPC_CREAT | 0660);
    
    // set the message type to 1 - can be any positive number (Indicates message is setPoint)
    mySetpointSend.mtype = 1;
    //Take initial setpoint from commandline argument
    int setpoint = atoi(argv[1]); //./Client *YOURSETPOINT*
    
    mySetpointSend.setpoint = setpoint;
    status = msgsnd(queue, &mySetpointSend, sizeof(mySetpointSend.setpoint), 0);

    while(1)
    {
        printf("Enter setpoint (in degrees):\n");  
        scanf("%d", &(msg01.setpoint))
        status = msgsnd(queue, &mySetpointSend, sizeof(mySetpointSend.setpoint), 0);
        wait_1_second()
    }
	
    return 0;
}
