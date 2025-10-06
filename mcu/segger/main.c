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

#define SIGNAL_A PA2
#define SIGNAL_B PA3    // TODO: replace with real pin values
#define TOT_ROT 120     // total number of pulses to make 1 rotation
#define TIMER TIM16

int counter     = 0;
int clockwise   = 0;
int timer_count = 0;

/*********************************************************************
 *
 *       main()
 *
 *  Function description
 *   Application entry point.
 */
int main(void) {
  // set up timers/counters, GPIO pins (x2)

  // flash, clock
  configureClock();    // SYS_CLK runs at 80MHz
  configureFlash();
  // divide clock down??
  counter = 0;    // initialize counter to 0

  // enable pins
  gpioEnable(GPIO_PORT_A);
  pinMode(SIGNAL_A, GPIO_INPUT);
  pinMode(SIGNAL_B, GPIO_INPUT);
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD2, 0b01);    // Set PA2 as pull-up

  // initialize timer 16
  // TODO: which timer do I want enabled?
  RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
  initTIM(TIMER);

  // 1. Enable SYSCFG clock domain in RCC
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // 2. Configure EXTICR for the input button interrupt
  SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI2, 0b000);    // Select PA2
  SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI3, 0b000);    // Select PA3

                                                                 // Enable interrupts globally
  __enable_irq();

  // configure mask but
  //  enable rising edge trigger for both
  EXTI->RTSR1 |= (1 << gpioPinOffset(SIGNAL_A));
  EXTI->RTSR1 |= (1 << gpioPinOffset(SIGNAL_B));

  // enable falling edge trigger for both
  EXTI->FTSR1 |= (1 << gpioPinOffset(SIGNAL_A));    // Enable falling edge trigger
  EXTI->FTSR1 |= (1 << gpioPinOffset(SIGNAL_B));    // Enable falling edge trigger
  // turn on EXTI2 and EXTI3 interrupt in NVIC_ISER
  // TODO: is this correct?
  NVIC->ISER[0] |= (1 << EXTI2_IRQn);    // SEND BOTH TO THE SAME INTERRUPT HANDLER
  NVIC->ISER[0] |= (1 << EXTI2_IRQn);

  // TODO: IRQ handler: if interrupt triggered, increment counter
  // check correct interrupt
  // clear interrupt
  // increment counter
  // TODO: just one counter variable ?

  // run for a certain amount of time (divide)
  // implement decoder algorithm

  float speed;
  while (1) {
    // calculate speed using whatever value is in timer
    speed = 1.0/(TOT_ROT*((4.0*timer_count))); //TODO: ADJUST SPEED CALCS
    // display speed and direction
    if(clockwise){
      printf("clockwise at %f", speed);
    }
    else {
      printf("counter-clockwise at %f", speed);
    }
    //delay here?
  }
}

void EXTI2_IRQHandler(void) {
  int A = digitalRead(SIGNAL_A);
  int B = digitalRead(SIGNAL_B);

  if (EXTI->PR1 & (1 << EXTI2_IRQn)) {    // TODO: check interrupt. CORRECT?
    // If so, clear the interrupt (NB: Write 1 to reset.)
    EXTI->PR1 |= (1 << EXTI2_IRQn);
    // TODO: assign clockwise/counterclockwise
    clockwise = (A && B) || (!A && !B);
  }
  if (EXTI->PR1 & (1 << EXTI3_IRQn)) {    // TODO: check interrupt. CORRECT?
    // If so, clear the interrupt (NB: Write 1 to reset.)
    EXTI->PR1 |= (1 << EXTI3_IRQn);
    clockwise = !((A && B) || (!A && !B));
  }

  counter = counter + 1;

  if (counter == TOT_ROT) {
    // check timer value and reset timer
    timer_count = TIMER->CNT;    // get value of timer
    TIMER->CNT  = 0;
    TIMER->EGR |= (1 << 0);
  }
}

void EXTI3_IRQHandler(void) {
  if (EXTI->PR1 & (1 << EXTI3_IRQn)) {    // TODO: check interrupt. CORRECT?
    // If so, clear the interrupt (NB: Write 1 to reset.)
    EXTI->PR1 |= (1 << EXTI3_IRQn);
  }
  counter = counter + 1;
  // TODO: check if counter is at 120 here?? probably..
}

uint32_t velocity_calc(int count, int time) {}
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