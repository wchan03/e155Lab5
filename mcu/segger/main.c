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
// TODO: include all necessary header files
#include "STM32L432KC.h"
#include <stm32l432xx.h>

#define SIGNAL_A PA1    // A1
#define SIGNAL_B PA2    // A7
#define TOT_ROT 408     // PPR (total number of pulses to make 1 rotation)
#define TIMER TIM6

int counter     = 0;
int clockwise   = 0;
int timer_count = 1;

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

/*********************************************************************
 *
 *       main()
 *
 *  Function description
 *   Application entry point.
 */
int main(void) {

  counter = 0;    // initialize counter to 0

  // enable pins
  gpioEnable(GPIO_PORT_A);
  pinMode(SIGNAL_A, GPIO_INPUT);
  pinMode(SIGNAL_B, GPIO_INPUT);
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD1, 0b01);    // Set PA1 as pull-up
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD2, 0b01);    // Set PA2 as pull-up

  // initialize timers
  // TODO: which timer do I want enabled?
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
  initTIM(TIMER); //initialized to run in milliseconds
  initTIM(TIM2); 

  // 1. Enable SYSCFG clock domain in RCC
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // 2. Configure EXTICR for the input button interrupt
  SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI1, 0b000);    // Select PA1
  SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI2, 0b000);    // Select PA2

  __enable_irq(); // Enable interrupts globally

  // configure mask bit
  EXTI->IMR1 |= (1 << gpioPinOffset(SIGNAL_A)); 
  EXTI->IMR1 |= (1 << gpioPinOffset(SIGNAL_B));

  //  enable rising edge trigger for both
  EXTI->RTSR1 |= (1 << gpioPinOffset(SIGNAL_A));
  EXTI->RTSR1 |= (1 << gpioPinOffset(SIGNAL_B));

  // enable falling edge trigger for both
  EXTI->FTSR1 |= (1 << gpioPinOffset(SIGNAL_A));
  EXTI->FTSR1 |= (1 << gpioPinOffset(SIGNAL_B));

  // turn on EXTI1 and EXTI2 interrupt in NVIC_ISER
  // TODO: is this correct?
  NVIC->ISER[0] |= (1 << EXTI1_IRQn);    // SEND BOTH TO THE SAME INTERRUPT HANDLER
  NVIC->ISER[0] |= (1 << EXTI2_IRQn);

  float speed = 0;
  while (1) {
    printf("hello");
    // calculate speed using whatever value is in timer
    speed = 1.0 / (TOT_ROT * ((4.0 * timer_count *1000.0)));    // TODO: ADJUST SPEED CALCS
    // display speed and direction
    if (clockwise) {
      printf("clockwise at %f rev/s %d \n", speed, timer_count);
    } else if(!clockwise) {
      printf("counter-clockwise at %f rev/s %d \n", speed, timer_count);
    }
    else {printf("you have an error...%d \n", timer_count);}
    // delay here?
    //delay_millis(TIM2, 500); //delay for half a second
  }
}

/*********************************************************************
 *
 *       EXTI2_IRQHandler()
 *
 *  Function description
 *   Application entry point.
 */

// TODO: IRQ handler: if interrupt triggered, increment counter
void EXTI1_IRQHandler(void) {

  int A = digitalRead(SIGNAL_A);
  int B = digitalRead(SIGNAL_B);
  
  if (EXTI->PR1 & (1 << EXTI1_IRQn)) {    // TODO: check interrupt. CORRECT?
    // If so, clear the interrupt (NB: Write 1 to reset.)
    EXTI->PR1 |= (1 << EXTI1_IRQn);
    // TODO: assign clockwise/counterclockwise
    clockwise = (A && B) || (!A && !B);
    // Then increment counter
    counter = counter + 1;
  }

  if (counter == (TOT_ROT*4)) {
    // check timer value and reset timer
    timer_count = TIMER->CNT;    // get value of timer
    TIMER->CNT  = 0;
    TIMER->EGR |= (1 << 0);
  }

}

void EXTI2_IRQHandler(void) {

  int A = digitalRead(SIGNAL_A);
  int B = digitalRead(SIGNAL_B);

  if (EXTI->PR1 & (1 << EXTI2_IRQn)) {    // TODO: check interrupt. CORRECT?
    // If so, clear the interrupt (NB: Write 1 to reset.)
    EXTI->PR1 |= (1 << EXTI2_IRQn);
    clockwise = !((A && B) || (!A && !B)); //assign counter clockwise
    // Then increment counter
    counter = counter + 1;
  }


  if (counter == (TOT_ROT*4)) { //TODO: is it total signals * 4
    // check timer value and reset timer
    timer_count = TIMER->CNT;    // get value of timer
    TIMER->CNT  = 0;
    TIMER->EGR |= (1 << 0);
  }

}



/*************************** End of file ****************************/

/*
//TODO: my questions

what does the algorithm look like?
how exactly does this work with interrupts?
only need 1 counter to increment on each edge?
how does the timer work with all of this? does the timer stop counting when an interrupt happen?
what is a mask and why is it important?
per the button_interrupt code: where does the interupt
  handler get called? is it automatic based on the name of the function?


2 separate functions: interrupt starts on rising edge until falling edge and count the time. also count time between
rising edge 1 and rising edge 2 (2nd sensor)

2 sensors attached to 2 pins


interrupt is essentially a trigger that alerts the main code to do something else


timer happens outside of mcu
2 options:
1. count for x amount of time, then use the counter # as ur variable
  configure timer as an interrupt
2. count until the full amount of pulses per rotation (kavi says 120), then use the timer val to calculate velocity


you DO have to worry about direction
rising edge A, b is high, moving forward. and vice versa
rising edge A, b is low, moving backwards
truth table?


*/