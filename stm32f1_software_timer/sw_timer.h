#ifndef TIMER_CTL_H
#define TIMER_CTL_H

#include "stm32f10x.h"
#include "stddef.h"
#include "string.h"

#ifndef SystemCoreClock
#define SystemCoreClock 72000000
#endif

#define MAX_TIMERS					8 /*amount of sw timers*/



#define DWT_CONTROL					((volatile uint32_t *)0xE0001000)
#define DWT_CYCCNT					((volatile uint32_t *)0xE0001004)
#define DEMCR						((volatile uint32_t *)0xE000EDFC)
#define LAR							((volatile uint32_t *)0xE0001FB0)

/*timers state*/
#define TIMER_FREE					0
#define TIMER_IDLE					1
#define TIMER_RUNNING				2
#define TIMER_PAUSED				3
/*infinite cycles constant*/
#define TIMER_CYCLES_INFINITE		0xFF


typedef void (*timer_callback_t)(void * param);

typedef struct
{
	uint32_t val_ms;				/* amount of milliseconds */
	uint32_t end_val_ms;			/* end values of global ms timer */
	uint32_t elapsed_time_ms;		/* elapsed time before pause */
	uint32_t cycles;				/* amount of event cycles */
	uint8_t state;					/* current timer state */
	timer_callback_t fn;					/* callback pointer */
	void * callback_param;					/* callback param pointer */
}timer_t; /*timer struct*/


static volatile uint32_t ms_var;

timer_t timers_array[MAX_TIMERS]; /*timers array*/

void ClockInit72MhzHSE(void);
void timers_init(void);
timer_t * add_timer(void);
timer_t * free_timer(timer_t * timer);
void timers_handler();
void delay_ms(uint32_t ms);
void delay_us(uint16_t us);
timer_t * timer_start(timer_t * timer, uint32_t ms, uint32_t cycles, timer_callback_t callback_function, void * callback_fn_param);
timer_t * timer_stop(timer_t * timer);
timer_t * timer_pause(timer_t * timer);
timer_t * timer_continue(timer_t * timer);
uint32_t get_core_ticks(void);


#endif
