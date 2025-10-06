//header for main.c

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "STM32L432KC.h"
#include <stm32l432xx.h>

#define SIGNAL_A PA1    // A1
#define SIGNAL_B PA2    // A7
#define PPR 408     // PPR (total number of pulses to make 1 rotation)
#define TIMER TIM6
#define duration 500

int counter     = 0;
int clockwise   = 0;
int timer_count = 1;
float speed = 0.0;

// Function Prototypes
int _write(int file, char *ptr, int len); //used for printf
void EXTI1_IRQHandler(void); //interrupt handler for PA1
void EXTI2_IRQHandler(void); //interrupt handler for PA2
void enablePins(void);

#endif