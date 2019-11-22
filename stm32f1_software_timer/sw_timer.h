#ifndef TIMER_CTL_H
#define TIMER_CTL_H

#include "stm32f10x.h"
#include "stddef.h"
#include "string.h"

#ifndef SystemCoreClock
#define SystemCoreClock 72000000
#endif

#define MAX_TIMERS		8 /*amount of sw timers*/


/*timers state*/
#define TIMER_FREE		0
#define TIMER_IDLE		1
#define TIMER_TICKING	2
#define TIMER_DONE		3


typedef void (*timer_callback_t)(void * param);

typedef struct
{
	uint32_t end_val_ms;
	volatile uint8_t state;
	timer_callback_t fn;
	void * callback_param;
}timer_t; /*timer struct*/

timer_t timers_array[MAX_TIMERS]; /*timers array*/
volatile uint32_t global_tick_ms_var; /*millisecond tick var, advancing by systick handler*/


void ClockInit72MhzHSE(void); 
void timers_init(void);		
timer_t * add_timer(void);	
timer_t * free_timer(timer_t * timer); 
void timers_handler();	


void delay_ms(uint16_t ms); /**/
void timer_start(uint16_t ms, timer_t * timer, timer_callback_t callback_function, void * callback_fn_param); /*start timer with defined time, callback and params*/

/*EXAMPLE

void func1 (void * data)
{
	*data = 1;
}


int main(void)
{

	uint8_t flag = 0;
	timer_t * timeout_tim = add_timer();
	timers_init();
	
	delay_ms(1000);
	
	while(1)
	{
		flag = 0;
		timer_start(1000, timeout_tim, func1, (void*)&flag)
		while(!flag)
		{
			timers_handler();
		}
	}

}



*/


#endif
