
#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>
#include "tm4c123gh6pm.h"

// REQUIRED: correct these bitbanding references for the off-board LEDs
#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 0*4))) // off-board red LED      PD0
#define ORANGE_LED   (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 1*4))) // off-board orange LED   PD1
#define YELLOW_LED   (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 3*4))) // off-board yellow LED   PD3
#define GREEN_LED    (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 2*4))) // off-board green LED    PD2
#define BLUE_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 2*4))) // ON BOARD BLUE
#define ONBOARD_GREEN_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))
#define ONBOARD_RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))

// PUSH BUTTONS
#define PUSHBUTTON1    (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 1*4))) //  PE1
#define PUSHBUTTON2    (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 2*4))) //  PE2
#define PUSHBUTTON3    (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 3*4))) //  PE3
#define PUSHBUTTON4    (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 4*4))) //  PE4
#define PUSHBUTTON5    (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 5*4))) //  PE5



// function pointer
typedef void (*_fn)();

int rtosScheduler(void);
void rtosInit(void);
void* fetchSP(void);
void writeSP(void* z);
void* getR0(void);
void rtosStart(void);
bool createThread(_fn fn, char name[], int priority);
uint8_t pidToTask(uint32_t pgId);
void destroyThread(_fn fn);
void setThreadPriority(_fn fn, uint8_t priority);
struct semaphore* createSemaphore(uint8_t count, char SemName[]);
void yield();
void sleep(uint32_t tick);
void wait(struct semaphore *pSemaphore);
void post(struct semaphore *pSemaphore);
void systickIsr();
void svCallIsr();
void pendSvIsr();

#endif /* KERNEL_H_ */
