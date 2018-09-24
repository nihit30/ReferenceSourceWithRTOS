// RTOS Framework - Fall 2017
// J Losh

// Student Name: NIHIT BHAVSAR
// TO DO: Add your name on this line.  Do not include your ID number.

// References :
// https://en.wikibooks.org/wiki/Embedded_Systems/RTOS_Implementation
// https://en.wikibooks.org/wiki/Embedded_Control_Systems_Design/Real_Time_Operating_systems
//

// Submit only two .c files in an e-mail to me (not in a compressed file):
// xx_rtos_coop.c   Single-file with cooperative version of your project
// xx_rtos_prempt.c Single-file with premptive version of your project
// (xx is a unique number that will be issued in class)
// Please do not include .intvecs section in your code submissions
// Please do not change any function names in this code or thread priorities

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// 5 Pushbuttons and 5 LEDs, UART

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "kernel.h"
#include "tasks.h"
#include "wait.h"
#include "initHw.h"
#include "lcdDriver.h"



//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

    int main(void)
    {
        bool ok;
        putsGraphicsLcd("Welcome to A-LPS");
        // Initialize hardware
        initHw();

        // Power-up flash
        RED_LED = 1;
        waitMicrosecond(50000);
        RED_LED = 0;
        waitMicrosecond(1000);
        rtosInit();

        // Add required idle process
        ok =  createThread(idle, "Idle", 15);

          // Add other processes
          ok &= createThread(debounce, "Debounce", 8);

        // Start up RTOS
        if (ok)

            rtosStart(); // never returns

        else
            RED_LED = 1;

        return 0;
        // don't delete this unreachable code
        // if a function is only called once in your code, it will be
        // accessed with two goto instructions instead of call-return,
        // so any stack-based code will not function correctly
        yield();
        sleep(0);
        wait(0);
        post(0);
    }
