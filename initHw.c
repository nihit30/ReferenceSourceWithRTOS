/*
 * initHw.c
 *
 *  Created on: Sep 24, 2018
 *      Author: nihit
 */
#include <stdint.h>
#include "tm4c123gh6pm.h"


// Initialize Hardware
    void initHw()
    {
        // REQUIRED: Add initialization for blue, orange, red, green, and yellow LEDs
        //           5 pushbuttons, and uart

        SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN
                | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);
        SYSCTL_GPIOHBCTL_R = 0;
        SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOA | SYSCTL_RCGC2_GPIOB
                | SYSCTL_RCGC2_GPIOC | SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOE
                | SYSCTL_RCGC2_GPIOF;

        // LED CONFIG
        GPIO_PORTD_DIR_R |= 0x0F;  // make bits 0,1,2,3 as outputs
        GPIO_PORTD_DR2R_R |= 0x0F; // set drive strength to 2mA
        GPIO_PORTD_DEN_R |= 0x0F;  // enable bits for digital operations
        // PUSH BUTTONS
        GPIO_PORTE_DIR_R |= 0x00;
        GPIO_PORTE_DR2R_R |= 0xFF; // set drive strength to 2mA
        GPIO_PORTE_DEN_R |= 0xFF;  // enable bits 4 and 5 for digital operations
        GPIO_PORTE_PUR_R |= 0x3E;  // internal pull ups for push button

        // ON BOARD LEDS
        GPIO_PORTF_DIR_R |= 0x0F; // bits 0,1,2,3 are outputs, other pins are inputs
        GPIO_PORTF_DR2R_R |= 0xFF; // set drive strength to 2mA (not needed since default configuration -- for clarity)
        GPIO_PORTF_DEN_R |= 0xFF;  // enable LEDs and pushbuttons

        SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R0; // turn-on UART0, leave other uarts in same status
        GPIO_PORTA_DEN_R |= 3;                     // default, added for clarity
        GPIO_PORTA_AFSEL_R |= 3;                   // default, added for clarity
        GPIO_PORTA_PCTL_R = GPIO_PCTL_PA1_U0TX | GPIO_PCTL_PA0_U0RX;

        UART0_CTL_R = 0;             // turn-off UART0 to allow safe programming
        UART0_CC_R = UART_CC_CS_SYSCLK;             // use system clock (40 MHz)
        UART0_IBRD_R = 21; // r = 40 MHz / (Nx115.2kHz), set floor(r)=21, where N=16
        UART0_FBRD_R = 45;                              // round(fract(r)*64)=45
        UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN; // configure for 8N1 w/ 16-level FIFO
        UART0_CTL_R = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN; // enable TX, RX, and module

        SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R5;     // turn-on timer
        WTIMER5_CTL_R &= ~TIMER_CTL_TAEN; // turn-off counter before reconfiguring
        WTIMER5_CFG_R = 4;               // configure as 32-bit counter (A only)
        WTIMER5_TAMR_R = TIMER_TAMR_TAMR_CAP | TIMER_TAMR_TACDIR; // configure for edge count mode, count up
        WTIMER5_CTL_R = 0;                               //
        WTIMER5_IMR_R = 0;                               // turn-off interrupts
        WTIMER5_TAV_R = 0;                      // zero counter for first period
//WTIMER5_CTL_R |= TIMER_CTL_TAEN;                 // turn-on counter
        NVIC_EN3_R &= ~(1 << (INT_WTIMER5A - 16 - 96)); // turn-off interrupt 120 (WTIMER5A)
    }

