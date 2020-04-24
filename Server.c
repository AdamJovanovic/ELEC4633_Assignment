// ELEC4633 Lab02 - Linux Server Program


#include <stdio.h>
#include <sys/msg.h>
#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>

#define READLENGTH 100
#define WRITELENGTH 100

int *readIndexMR_c;
int *writeIndexMR_c;
int *setpoint;


int *motorReadBuffer_c;

struct mymsgbuf {
    long mtype;
    int setpoint;
};

struct ..............;

int motorReadBuffer_len = READLENGTH;
int motorWriteBuffer_len = WRITELENGTH;

int main(void)
{
    int status,i;
    int rqueue;
    key_t msgkey;
    
    // generate the message key to obtain a queue id - same as in send program
    msgkey = ftok(".",'m');

  // obtain a queue id - should be same id as in send program
    rqueue = msgget(msgkey, IPC_CREAT | 0660);
    
 	..................................................
 
    while(1)
    {
     /* Add code */
    int id;
    for(id=0;id<50;id++)
    {
        if(*readIndexMR_c==...............)
        {break;}
        printf("value at index %d is %d \n",*readIndexMR_c,motorReadBuffer_c[*readIndexMR_c]);
        (*readIndexMR_c)++;
        if(*readIndexMR_c==.................)
        {   *readIndexMR_c=0; }
        
     }
     
     status = msgrcv(.........,........., sizeof(rmsg01.setpoint), 0, 0);
     if(rmsg01.mtype==1)
     {
     *setpoint = rmsg01..........;
     }
     usleep(5000000);
    }
    
	.................................
	
  return 0;
}
