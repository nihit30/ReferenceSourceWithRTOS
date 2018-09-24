/*
 * tasks.c
 *
 *  Created on: Sep 24, 2018
 *      Author: nihit
 */

#include <stdint.h>
#include <stdbool.h>
#include "wait.h"
#include "kernel.h"

// ------------------------------------------------------------------------------
//  Task functions
// ------------------------------------------------------------------------------

// one task must be ready at all times or the scheduler will fail
// the idle task is implemented for this purpose
    void idle()
    {
        while (true)
        {
            //tcb[taskCurrent].state = STATE_READY;
            ORANGE_LED = 1;
            waitMicrosecond(1000);
            ORANGE_LED = 0;
            yield();
        }
    }

    // REQUIRED: add code to return a value from 0-31 indicating which of 5 PBs are pressed
        uint8_t readPbs()
        {
            uint32_t a = 0x00;
            if (PUSHBUTTON1 == 0)
            {
                a = 1;

            }
            else if (PUSHBUTTON2 == 0)
            {
                a = 2;

            }
            if (PUSHBUTTON3 == 0)
            {
                a = 4;

            }
            if (PUSHBUTTON4 == 0)
            {
                a = 8;

            }
            if (PUSHBUTTON5 == 0)
            {
                a = 16;

            }

            return a;

        }


    void debounce()
    {
        uint8_t count;
        while (true)
        {
            //wait(keyPressed);
            count = 10;
            while (count != 0)
            {
                sleep(10);
                if (readPbs() == 0)
                    count--;
                else
                    count = 10;
            }
            //post(keyReleased);
        }
    }



