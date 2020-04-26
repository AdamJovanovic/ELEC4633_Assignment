// ELEC4633 Lab02 - Linux Server Program

#include <stdio.h>
#include <sys/msg.h>
#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>

/////////////////////////////////// DEFINES ////////////////////////////////////////

#define DISPLAY_LENGTH 50
#define CIRCULAR_BUFFER_SIZE 100

#define LOWER_HALF 0
#define UPPER_HALF 1

#define WAIT_5_SECONDS 5000000

/////////////////////////////// FUNCTION MACROS ////////////////////////////////////

#define wait_5_seconds() usleep(WAIT_5_SECONDS)

/////////////////////////////// GLOBAL VARIABLES ///////////////////////////////////

int *setpoint;

typedef struct circularbuffer
{
  int contents[CIRCULAR_BUFFER_SIZE];
  int writeIndex;   //Indicates where the RTAI task is currently up to.
} CircularBuffer;

typedef struct setpointmessage {
    long mtype;
    int setpoint;
} SetpointMessage;

int motorReadBuffer_len = READLENGTH;
int motorWriteBuffer_len = WRITELENGTH;

//////////////////////////////////// MAIN /////////////////////////////////////////

int main(void)
{
    CircularBuffer *myCircBuffer = rtai_malloc(nam2num("read_shmem"), sizeof(CircularBuffer));
    setpoint = rtai_malloc(nam2num("setpoint_shmem"), sizeof(int));
    SetpointMessage mySetpointRcvd;   //Declare message struct (Setpoint) FROM Client.c

    int status;
    int rqueue;
    key_t msgkey;

    int idx; //Counter variable for reading from the circular buffer
    int prevRead; //If prevRead:
                        // == 0, last read was from 0 - 49 i.e. Read 50 - 99 IF writeIndex reaches 0.
                        // == 1, last read was from 50 - 99

    // generate the message key to obtain a queue id - same as in send program
    msgkey = ftok(".",'m');

    // obtain a queue id - should be same id as in send program
    rqueue = msgget(msgkey, IPC_CREAT | 0660);
 
    while(1)
    {
        //Waits until the writeIndex value indicates that the upper or lower half has finished writing
        if ((prevRead == LOWER_HALF && myCircBuffer->writeIndex < DISPLAY_LENGTH) || 
            (prevRead == UPPER_HALF && myCircBuffer->writeIndex > DISPLAY_LENGTH))
        {
            do  //Will enter the loop while idx == 0 or 50
            {
                printf("Value at index %d is %d\n", idx, myCircBuffer->contents[idx]);
                idx++;
            } while (idx % DISPLAY_LENGTH != 0);
        }
        
        idx = idx % CIRCULAR_BUFFER_SIZE;    //Reset the read index at the limit of the circular buffer size
        
        //Check the message queue for messages addressed to the queue id.
        status = msgrcv(rqueue, &mySetpointRcvd, sizeof(mySetpointRcvd.setpoint), 0, 0);
        
        //Message type 1 is associated with setpoint. (Client.c sets mtype as 1 before sending)
        if(mySetpointRcvd.mtype==1)
        {
            *setpoint = mySetpointRcvd.setpoint; //Write new setpoint to shared memory.
        }

        //wait_5_seconds();
    }
    
	//Do something...???
	
  return 0;
}