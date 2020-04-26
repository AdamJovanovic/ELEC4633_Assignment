// ELEC4633 Lab02 - Comedi

#include <linux/module.h>
#include <linux/init.h>

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_shm.h>
#include <rtai_nam2num.h>

#include <linux/comedi.h>
#include <linux/comedilib.h>

#define ARG 0
#define STACK_SIZE 1024
#define PRIORITY RT_SCHED_HIGHEST_PRIORITY
#define USE_FPU 1
#define STOP 2048
#define NOW rt_get_time()

// Set the periods so they run at 10 Hz
#define PERIOD_1 nano2count(1e8) //1e8 Translates to 10Hz

// ADC/DAC related vriables
#define READ_SUBDEVICE 0
#define WRITE_SUBDEVICE 1
#define READ_CHANNEL 0
#define WRITE_CHANNEL 0
#define RANGE 0
#define AREF AREF_GROUND
#define STOP_MOTOR 2048

#define BUFFER_SIZE 100

/* Store data needed for the thread */
RT_TASK threadRead;         // thread which reads motor position/speed 

/* Data needed by comedi */
comedi_t *comedi_dev;

int motorRead;
int scaledMotorRead;
int motorWrite;

#define Kp 0.5              //Propotional gain (ALWAYS 0.5)
int *setPoint;              //Position setpoint

typedef struct transmitbuffer
{
  int contents[BUFFER_SIZE];
  int writeIndex;
} TXBuffer;

TXBuffer init_buffer()
{
    TXBuffer *myBuffer = malloc(sizeof(*myBuffer));
    if(!myBuffer)
    {
        perror("Failed to allocate memory for Circular Buffer");
        exit(1);
    }

    myBuffer->writeIndex = 0;
    return myBuffer
}

void write_buffer(int data, TXBuffer* aBuffer)
{
    aBuffer->contents[aBuffer->writeIndex] = data;
    aBuffer->writeIndex = (aBuffer->writeIndex + 1) % BUFFER_SIZE;
}

TXBuffer *txBuffer = init_buffer();

/* The code that is run */
void rtMotorRead(long arg)
{
    while (1)
    {
        comedi_data_read(comedi_dev, READ_SUBDEVICE, READ_CHANNEL, RANGE, AREF, &motorRead);

        //Scale the ADC value -> From 12-bit value to degrees. 2048 = 0
        if (motorRead > 2048 && motorRead < 4096) {
            scaledMotorRead = 180 + (((4096 - motorRead)*360)/4096); //2049 = 360 -> 1095 = 180
        } else { 
            scaledMotorRead = 180 - ((motorRead*360)/4096); //0 = 180 -> 2048 = 0
        }

        //Obtain the current motor position and write it into the read buffer.*/
        write_buffer(scaledMotorRead, TXBuffer); //Write the scaled value (As an angle) to the circular buffer
        
        int error = *setPoint - scaledMotorRead;
        int u = Kp * error;     //Control signal u = Kp * (setpoint - current position)

        if ( u = 0 )
        {
            motorWrite = STOP_MOTOR
        }
        else
        {
            if (u < 0) // Setpoint is located "behind", i.e. setpoint < scaled value
            {
                u = 360 - abs(u);
            }
            motorWrite = (u > 0 && u < 180) ? ((u * 4096) / 360) + 2048 : ((u * 4096) / 360) - 2048;
        }       

        comedi_data_write(comedi_dev, WRITE_SUBDEVICE, WRITE_CHANNEL, RANGE, AREF, motorWrite);

        printk("Raw motor position = %d\n", txBuffer->contents[txBuffer->writeIndex]);
        printk("Buffer index %d \n",txBuffer->writeIndex);// check whether data is properly read
        printk("Scaled position = %d, Setpoint = %d, Error = %d", scaledMotorRead, *setPoint, error);
        rt_task_wait_period();
    }
}

/* Called when "insmod" is used */
static int __init template_init(void)
{
    /* Start the RT timer, NB only needs to be done once */
    rt_set_periodic_mode();
    start_rt_timer(PERIOD_1);

    // Open Hardware
    comedi_dev = comedi_open("/dev/comedi0");

    /* Initialise the data associated with a thread and make it periodic */
    rt_task_init(&threadRead, rtMotorRead, ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);

    // Shared memory allocation to the variables

    txBuffer = rtai_kmalloc(nam2num("read_shmem"), BUFFER_SIZE * sizeof(int));
    setPoint = rtai_kmalloc(nam2num("setpoint_shmem"), sizeof(int));

    rt_task_make_periodic(&threadRead, NOW, PERIOD_1);
    
    return 0;
}

/* Called when "rmmod" is used */
static void __exit template_exit(void)
{
    rtai_kfree(nam2num("read_shmem"));
    rtai_kfree(nam2num("setpoint_shmem"));

    rt_task_delete(&threadRead);

    comedi_close(comedi_dev);
}

module_init(template_init);
module_exit(template_exit);
