#ifndef TIMER_CTL
#define TIMER_CTL

#include "stm32f10x.h"

#define TIMER_ENABLED	(TIM1->CR1 & (1 << 0))

void capture_timer(void);
void release_timer(void);
void delay_ms(uint16_t ms);
void timeout_ms(uint16_t ms);
void timer_init(void);

void ClockInit72MhzHSE();

volatile uint8_t timer_state; // 0 - free, 1 - busy


#endif
