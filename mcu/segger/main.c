/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : main.c
Purpose : Lab 5 Main Code
Author  : Wava Chan
Email   : wchan@g.hmc.edu
Date    : October 2025

*/

#include <stdio.h>
//TODO: include all necessary header files
#include "STM32L432KC.h"
#include <stm32l432xx.h>

#define SIGNAL_A 2
#define SIGNAL_B 3 //TODO: replace with real pin values

/*********************************************************************
*
*       main()
*
*  Function description
*   Application entry point.
*/
int main(void) {
  // set up timers/counters, GPIO pins (x2)
  //flash, clock
  configureClock(); //SYS_CLK runs at 80MHz
  configureFlash();
  //divide clock down??

  //enable pins
  gpioEnable(GPIO_PORT_A);

  gpioEnable(SIGNAL_B);

  //count on every rising/falling edge of BOTH signals/pins
  // if interrupt triggered, increment counter
  //TODO: just one counter variable 
  //run for a certain amount of time (divide)
  //implement decoder algorithm
  //TODO: what is my minimum speed? 

  //exit IRQ handler
}

/*************************** End of file ****************************/

/*
//TODO: my questions

what does the algorithm look like? 
how exactly does this work with interrupts?
only need 1 counter to increment on each edge?
how does the timer work with all of this? does the timer stop counting when an interrupt happen?


2 separate functions: interrupt starts on rising edge until falling edge and count the time. also count time between rising edge 1 and rising edge 2 (2nd sensor)

2 sensors attached to 2 pins


*/