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
#include "main.h"

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
  initTIM(TIMER);    // initialized to run in milliseconds
  initTIM(TIM2);

  // 1. Enable SYSCFG clock domain in RCC
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // 2. Configure EXTICR for the input button interrupt
  SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI1, 0b000);    // Select PA1
  SYSCFG->EXTICR[1] |= _VAL2FLD(SYSCFG_EXTICR1_EXTI2, 0b000);    // Select PA2

  __enable_irq();                                                // Enable interrupts globally

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
  NVIC->ISER[0] |= (1 << EXTI1_IRQn);    // SEND BOTH TO THE SAME INTERRUPT HANDLER?
  NVIC->ISER[0] |= (1 << EXTI2_IRQn);

  while (1) {
    // calculate speed using whatever value is in timer
    if (TIMER->CNT >= 200) {
      if (counter == 0) {
        speed = 0.0;
      } else {
      // # pings * 1 rev/# pings / duration (s) = rev/s
        speed = counter * (0.25) * (1000.0/TIMER->CNT) *(1.0/PPR); // TODO: ADJUST SPEED CALCS
      }
      //reset counters
      TIMER->CNT  = 0;
      TIMER->EGR |= (1 << 0);
      counter = 0;                 // reset counter
    }    
    // display speed and direction
    if (clockwise) {
      printf("clockwise at ");
      printf("%f  ", speed);
      printf("rev/s\n");    //, timer_count);
    } else if (!clockwise) {
      printf("counter-clockwise at %f rev/s \n", speed);
    } else {
      printf("you have an error...%d \n", timer_count);
    }

    //delay_millis(TIM2, 100);    // delay for half a second
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
void EXTI1_IRQHandler(void) {    // interrupt for signal A
  int A = digitalRead(SIGNAL_A);
  int B = digitalRead(SIGNAL_B);

  //if (EXTI->PR1 & (1 << EXTI1_IRQn)) {    // TODO: check interrupt. CORRECT?
  if(1){
    //  If so, clear the interrupt (NB: Write 1 to reset.)
    EXTI->PR1 |= (1 << EXTI1_IRQn);
    // TODO: assign clockwise/counterclockwise
    clockwise = !((A && B) || (!A && !B));
    // Then increment counter
    counter = counter + 1;
  }
}

void EXTI2_IRQHandler(void) {    // interrupt for signal B
  int A = digitalRead(SIGNAL_A);
  int B = digitalRead(SIGNAL_B); // we do reach this point

  //if (EXTI->PR1 & (1 << EXTI2_IRQn)) {    // TODO:i don't know if this ever gets triggered
  if (1) {
    // If so, clear the interrupt (NB: Write 1 to reset.)
    EXTI->PR1 |= (1 << EXTI2_IRQn);
    clockwise = ((A && B) || (!A && !B));    // assign counter clockwise
    // Then increment counter
    counter = counter + 1;
  }
}

/*************************** End of file ****************************/

/*
//TODO: my questions

two main issues:
1. my counter isn't incrementing. does my condition ever get triggered?
could it be i'm not connected to my pins correctly?
2. speed is not printing?


*/