
#include <stdint.h>
#include <stdbool.h>
#include "kernel.h"

// task
#define STATE_INVALID    0 // no task
#define STATE_UNRUN      1 // task has never been run
#define STATE_READY      2 // has run, can resume at any time
#define STATE_BLOCKED    3 // has run, but now blocked by semaphore
#define STATE_DELAYED    4 // has run, but now awaiting timer

#define MAX_TASKS 10       // maximum number of valid tasks
uint8_t taskCurrent = 0;   // index of last dispatched task
uint8_t taskCount = 0;     // total number of valid tasks
uint8_t tick;
uint32_t killprog;

// semaphore
#define MAX_SEMAPHORES 5
#define MAX_QUEUE_SIZE 5
#define MAX_CHARS 20

char shellBuffer[MAX_CHARS];
uint32_t pidMatch;
uint8_t taskNo;
uint8_t counter = 0;
uint8_t field_posi[10];
char field_type[10];

struct semaphore
{
    uint16_t count;
    uint16_t queueSize;
    uint32_t processQueue[MAX_QUEUE_SIZE]; // store task index here
    char SemaphoreName[30];
    uint8_t queueIndex;
} semaphores[MAX_SEMAPHORES];

uint8_t semaphoreCount = 0;

struct semaphore *keyPressed, *keyReleased, *flashReq, *resource;
struct semaphore *pSemaphore;
bool ok;

struct _tcb
{
    uint8_t state;                 // see STATE_ values above
    void *pid;                     // used to uniquely identify thread
    void *sp;                      // location of stack pointer for thread
    uint8_t priority;              // 0=highest, 15=lowest
    uint8_t currentPriority;       // used for priority inheritance
    uint32_t ticks;                // ticks until sleep complete
    char name[16];                 // name of task used in ps command
    void *semaphore;     // pointer to the semaphore that is blocking the thread
    uint32_t processTime;
    uint8_t skipCount;
    char* list[10];

} tcb[MAX_TASKS];

uint32_t stack[MAX_TASKS][256]; //   256*10(max tasks)*4 bytes = 10.240 kB stack

//-----------------------------------------------------------------------------
// RTOS Kernel Functions
//-----------------------------------------------------------------------------

int rtosScheduler()
{
    bool ok;
    static uint8_t task = 0xFF;
    ok = false;
    while (!ok)
    {
        task++;
        if (task >= MAX_TASKS)
            task = 0;

        if (tcb[task].state == STATE_UNRUN)
        {
            break;
        }
        else if ((tcb[task].skipCount >= tcb[task].priority)
                && (tcb[task].state == STATE_READY))
        {
            tcb[task].skipCount = 0;
            break;
        }
        else
            ++tcb[task].skipCount;

    }
    return task;
}

void rtosInit()
{
    uint8_t i;
    // no tasks running
    taskCount = 0;
    // clear out tcb records
    for (i = 0; i < MAX_TASKS; i++)
    {
        tcb[i].state = STATE_INVALID;
        tcb[i].pid = 0;
    }
    // INITIALIZING SYSTICK
    NVIC_ST_CTRL_R = ~NVIC_ST_CTRL_ENABLE;
    NVIC_ST_RELOAD_R = 0x00009C3F;
    NVIC_ST_CURRENT_R = 0x00009C3F;
    NVIC_ST_CTRL_R = 0X07;

}

void* fetchSP()                              //  FETCH SP LOCATION IN C VARIABLE
{ // Reference :https://stackoverflow.com/questions/32953949/saving-value-of-stack-pointer-in-c-variable-in-code-composer-studio-for-arm-cort
    __asm(" mov r0, sp");
    __asm(" bx lr");
}

void writeSP(void* z)                                //  WRITE SP TO SP REGISTER
{ // Reference : https://stackoverflow.com/questions/32953949/saving-value-of-stack-pointer-in-c-variable-in-code-composer-studio-for-arm-cort
    __asm(" mov sp, r0");
    __asm(" bx lr");
}

void* getR0()
{
    __asm(" bx lr ");
}

void rtosStart()
{
    // REQUIRED: add code to call the first task to be run
    _fn fn;
    fn = (_fn) tcb[taskCurrent].pid;
    (*fn)();                                          // CALL IDLE AS FIRST TASK
    taskCurrent = rtosScheduler();
    // Add code to initialize the SP with tcb[task_current].sp;
    //writeSP(tcb[taskCurrent].sp);
}

bool createThread(_fn fn, char name[], int priority)
{

    __asm(" svc #0x05 ");
    return ok;

}

uint8_t pidToTask(uint32_t pgId)
{       // Reference : https://people.redhat.com/anderson/extensions/ps.c
    uint8_t j;
    for (j = 0; j < taskCount; j++)
    {
        if ((uint32_t) tcb[j].pid == pgId)
            break;
    }
    return j;
}

// REQUIRED: modify this function to destroy a thread

void destroyThread(_fn fn)
{
    __asm(" svc #0x06 ");

}

// REQUIRED: modify this function to set a thread priority
void setThreadPriority(_fn fn, uint8_t priority)
{
    uint8_t t = pidToTask((uint32_t) fn);
    tcb[t].priority = priority;
}

struct semaphore* createSemaphore(uint8_t count, char SemName[30])
{
    struct semaphore *pSemaphore = 0;
    // uint8_t i;
    if (semaphoreCount < MAX_SEMAPHORES)
    {
        pSemaphore = &semaphores[semaphoreCount++];
        //strcpy(pSemaphore->SemaphoreName, SemName);
        pSemaphore->count = count;
    }

    return pSemaphore;
}

// REQUIRED: modify this function to yield execution back to scheduler using pendsv
void yield()
{
    __asm(" svc #0x01 ");

}

// REQUIRED: modify this function to support 1ms system timer
// execution yielded back to scheduler until time elapses using pendsv
void sleep(uint32_t tick)
{
    // push registers, set state to delayed, store timeout, call scheduler, pop registers,
    __asm(" svc #0x03 ");
    // return to new function (separate unrun or ready processing)
}

// REQUIRED: modify this function to wait a semaphore with priority inheritance
// return if avail (separate unrun or ready processing), else yield to scheduler using pendsv
void wait(struct semaphore *pSemaphore)
{
    __asm(" svc #0x02 ");

}

// REQUIRED: modify this function to signal a semaphore is available using pendsv
void post(struct semaphore *pSemaphore)
{
    __asm(" svc #0x04 ");

}

// REQUIRED: Implement prioritization to 16 levels

// REQUIRED: modify this function to add support for the system timer
// REQUIRED: in preemptive code, add code to request task switch
void systickIsr()
{
    //bool delayedTask = false;
    uint8_t k = 0;
    uint32_t tickCount = 0;

    for (k = 0; k < taskCount; k++)
    {
        if (tcb[k].state == STATE_DELAYED)
        {
            tickCount = tcb[k].ticks;
            if (tickCount == 0)
            {
                tcb[k].state = STATE_READY;

            }
            else
                tcb[k].ticks--;

        }

    }
    NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
}

// REQUIRED: modify this function to add support for the service call
// REQUIRED: in preemptive code, add code to handle synchronization primitives
void svCallIsr()
{
    uint32_t arg1;
    uint32_t arg2;
    uint32_t arg3;
    arg1 = (uint32_t) getR0();
    __asm(" mov r0, r1 ");
    arg2 = (uint32_t) getR0();
    __asm(" mov r0, r2 ");
    arg3 = (uint32_t) getR0();
    __asm(" ldr r4, [sp, #80] ");
    // adjusting stack pointer to previous svc instruction and loading PC to r4
    __asm(" ldr r0, [r4,#-2] ");
    // fetching instruction from PC - 2
    //Reference : http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0203j/Cacdfeci.html

    uint32_t number = (0x000000ff & ((uint32_t) getR0())); // masking top 8 bits to get svc number
    struct semaphore *pSemaphore = 0;
    uint8_t j, i;
    uint8_t t = 0;

    char* name;

    uint8_t priority;
    _fn fn;

    switch (number)
    {
    case 1:
        NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;        //  yield
        break;

    case 2:                               // wait

        pSemaphore = (struct semaphore*) arg1;

        if (pSemaphore->count > 0)
        {
            pSemaphore->count--;
            return;
        }
        else
        {

            tcb[taskCurrent].state = STATE_BLOCKED;
            pSemaphore->processQueue[pSemaphore->queueSize++] =
                    (uint32_t) tcb[taskCurrent].pid;

            tcb[taskCurrent].semaphore = pSemaphore;
            NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;

        }
        break;

    case 3:
        //sleep
        tcb[taskCurrent].ticks = tick;
        tcb[taskCurrent].state = STATE_DELAYED;
        NVIC_INT_CTRL_R |= NVIC_INT_CTRL_PEND_SV;
        break;

    case 4:
        //post
        pSemaphore = (struct semaphore*) arg1;

        pSemaphore->count++;
        if (pSemaphore->queueSize != 0)
        {
            j = pidToTask((uint32_t) pSemaphore->processQueue[0]);

            for (i = 0; i < pSemaphore->queueSize; i++)
            {
                if (tcb[j].priority
                        >= tcb[pidToTask(pSemaphore->processQueue[i + 1])].priority)
                    tcb[j].priority = tcb[pidToTask(
                            pSemaphore->processQueue[i + 1])].priority;
            }

            tcb[j].state = STATE_READY;
            pSemaphore->count--;
            pSemaphore->queueSize--;

            for (i = 0; i < taskCount; i++)
            {
                pSemaphore->processQueue[i] = pSemaphore->processQueue[i + 1]; // shifting elements in array by writing future values to present
            }
        }
        return;

    case 5:

        fn = (_fn) arg1;
        name = (char*) arg2;
        priority = arg3;

        //volatile uint8_t k = 0;
        bool found = false;
        uint8_t z;
        uint8_t i = 0;

        // REQUIRED: store the thread name
        // add task if room in task list

        i = 0;
        if (taskCount < MAX_TASKS)
        {
            // make sure fn not already in list (prevent reentrancy)
            while (!found && (i < MAX_TASKS))
            {
                found = (tcb[i++].pid == fn); // if function pointer = pid then found is true and exits loop.
            }
            if (!found)
            {
                // find first available tcb record
                i = 0;
                while (tcb[i].state != STATE_INVALID)
                {
                    i++;
                }
                tcb[i].state = STATE_UNRUN;
                tcb[i].pid = fn;                        // pid = PC
                tcb[i].sp = &stack[i][255];
                tcb[i].priority = priority;
                tcb[i].currentPriority = priority;
                for (z = 0; z < 16; z++)
                {
                    tcb[i].name[z] = name[z];
                }
                // increment task count
                taskCount++;
                ok = true;
            }
        }
        // REQUIRED: allow tasks switches again

        break;

    case 6:                              // destroy thread

        fn = (_fn) arg1;

        for (i = 0; i < taskCount; i++)
        {
            if (tcb[i].pid == fn)
            {
                tcb[i].state = STATE_INVALID;
                tcb[i].pid = 0;
                taskCount--;
                pSemaphore = tcb[i].semaphore;
                tcb[i].pid = 0;
                pSemaphore->count++;
                for (t = 0; t <= 5; t++) // clear out the semaphores the task has been waiting on
                {
                    if (pSemaphore->processQueue[t] == (uint32_t) tcb[killprog].pid)
                        for (t = 0; t <= pSemaphore->queueSize; t++)
                        {
                            pSemaphore->processQueue[t] =
                                    pSemaphore->processQueue[t + 1];
                        }

                }
                pSemaphore->queueSize--;
            }
        }

        break;
    default:
        break;

    }
}

// REQUIRED: in coop and preemptive, modify this function to add support for task switching
// REQUIRED: process UNRUN and READY tasks differently

void *z;
void pendSvIsr()
{

    __asm(" push {r4-r11}");
    tcb[taskCurrent].sp = fetchSP();
    taskCurrent = rtosScheduler();
    writeSP(tcb[taskCurrent].sp);

    switch (tcb[taskCurrent].state)
    {
    case STATE_READY:
    {
        __asm(" pop { r4-r11 } ");
        __asm(" mvn lr, #~0xFFFFFFF9 ");
        break;
    }

    case STATE_UNRUN:
    {
        tcb[taskCurrent].state = STATE_READY;
        writeSP(&stack[taskCurrent][246]);   // ADJUST SP TO CURRENT TASK SP
        stack[taskCurrent][247] = 0xFFFFFFF9;
        stack[taskCurrent][253] = (uint32_t) tcb[taskCurrent].pid; // changes
        stack[taskCurrent][254] = (uint32_t) tcb[taskCurrent].pid;
        stack[taskCurrent][255] = 0x01000000;
        break;
    }

    }
}

