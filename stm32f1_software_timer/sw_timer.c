#include "sw_timer.h"

/*
@ brief :  initialize system clock, systick and clear timers struct array. Should called before work.
@ param : none
@ retval : none
*/
void timers_init(void)
{
	ms_var = 0;
	ClockInit72MhzHSE();
	SysTick_Config(SystemCoreClock / 1000);
	memset(timers_array,'\0', sizeof(timers_array));


	/* unlock access to DWT (ITM, etc.)registers */
	*LAR = 0xC5ACCE55;
	/* enable the use DWT */
	*DEMCR = *DEMCR | 0x01000000;
	/* Reset cycle counter */
	*DWT_CYCCNT = 0;
	/* enable cycle counter */
	*DWT_CONTROL |= 1 ;
}

/*
@ brief : Adding new timer to (timer_t timers_array[MAX_TIMERS];) If there free slot available.
@ param : none
@ retval : return pointer to free timer struct, if there is no slots available - return NULL
*/
timer_t * add_timer(void)
{
	uint8_t i = 0;
	while (i < MAX_TIMERS)
	{
		 if (timers_array[i].state) /*searching for free timer*/
			 i++;
		 else /*free timer found, occupy it and fill struct with param*/
		 {
			 timers_array[i].state = TIMER_IDLE;
			 return &timers_array[i];
		 }
	}
	return NULL; /*all timers is occupied*/
}

/*
@ brief : release unused timer from array and free slot for new timer
@ param : pointer on a timer struct what should be freed
@ retval : pointer on a timer struct what is freed by this call or NULL if timer already free or bad ptr.
*/
timer_t * free_timer(timer_t * timer)
{
	if (timer->state)
	{
		memset (timer, '\0', sizeof(timer_t));
		return timer;
	}
	else
		return NULL;
}

/*
@ brief : Handler function. Check what timers is finished their count and invoke callbacks for this timers if its assigned.
@		If there is no callback - just set flag (TIMER_IDLE) if no cycles left in (volatile uint8_t state;) field in current structure.
@		This function should be called time to time or in Idle cycle.
@ param : none
@ retval : none
*/
void timers_handler(void)
{
	for (uint8_t i = 0;i < MAX_TIMERS; i++) /*timers passthrough*/
	{
		if (timers_array[i].state != TIMER_RUNNING) /*skip idle, paused and non used timers*/
			continue;
		//if((ms_var - timers_array[i].end_val_ms) >= timers_array[i].val_ms)
		if(ms_var >= timers_array[i].end_val_ms) /*timeout - callback execution*/
		{
			if (timers_array[i].fn)
				timers_array[i].fn(timers_array[i].callback_param); /*use callback with assigned parameters*/

			/*cycles handling*/
			if (timers_array[i].cycles == TIMER_CYCLES_INFINITE)
			{
				timers_array[i].end_val_ms  = timers_array[i].val_ms + ms_var; /*reload timer*/
				continue;
			}
			timers_array[i].cycles -=1;
			if(!timers_array[i].cycles) /*if no cycles left, timer is done*/
				timers_array[i].state = TIMER_IDLE; /*disable timer*/
			else /*else reloading timer for new cycle*/
			{
				timers_array[i].end_val_ms  = timers_array[i].val_ms + ms_var; /*reload timer*/
			}
		}
	}
}

/*
@ brief : Systick handler advance "ms_var" variable for timers
@ param : none
@ retval : none
*/
void SysTick_Handler(void)
{
	ms_var++;
}

/*
@ brief : simple delay function, no need to create timer for use. Just stops code execution on specified amount of time.
@ param : milliseconds
@ retval : none
*/
void delay_ms(uint32_t ms)
{
	volatile uint32_t end = ms_var + ms;
	while(ms_var != end)
		asm("nop");
}

/*
@ brief : delay function what used DWT register and free running core clock counter,
@		 no need to create timer for use. Just stops code execution on specified amount of time.
@ param : microseconds
@ retval : none
*/
void delay_us(uint16_t us)
{
	volatile uint32_t core_ticks_fin = (*DWT_CYCCNT) + (us * 72);
	while ((core_ticks_fin - (*DWT_CYCCNT)) < (us * 72))
		asm("nop");
}

/*
@ brief : Function for starting/restarting timer with defined parameters. Cycles can be infinite (pass TIMER_CYCLES_INFINITE constant in cycles param)
@ param : Timer pointer, milliseconds for timeout, amount of cycles, callback function pt, callback parameters pt.
@ retval : timer ptr what was started or restarted, NULL when timer is not used or bad pointer
*/
timer_t * timer_start(timer_t * timer, uint32_t ms, uint32_t cycles, timer_callback_t callback_function, void * callback_fn_param)
{
	if (timer->state)
	{
		timer->val_ms = ms;
		timer->end_val_ms = ms + ms_var;
		timer->cycles = cycles;
		timer->fn = callback_function;
		timer->callback_param = callback_fn_param;
		timer->state = TIMER_RUNNING;
		return timer;
	}
	else
		return NULL;
}

/*
@ brief : Stop timer function
@ param : timer struct pointer what should be stopped
@ retval : timer struct pointer what was stopped, or NULL if timer not used or bad pointer
*/
timer_t * timer_stop(timer_t * timer)
{
	if(timer->state)
	{
		timer->state = TIMER_IDLE;
		return timer;
	}
	else
		return NULL;
}

/*
@ brief : Pause timer function.
@ param : timer struct pointer what should be paused
@ retval : timer struct pointer what was paused, if timer is not running or timer end_val_ms expired return NULL
*/
timer_t * timer_pause(timer_t * timer)
{
	if(timer->state == TIMER_RUNNING)
	{
		if (ms_var >= timer->end_val_ms)
		{
			return NULL;
		}
		timer->elapsed_time_ms = timer->end_val_ms - ms_var;
		timer->state = TIMER_PAUSED;
		return timer;
	}
	else
		return NULL;
}

/*
@ brief : Continue timer function
@ param : timer struct pointer what should be continued
@ retval : timer struct pointer what was continued or NULL if timer is not stopped.
*/
timer_t * timer_continue(timer_t * timer)
{
	if(timer->state == TIMER_PAUSED)
	{
		timer->end_val_ms = (timer->val_ms - timer->elapsed_time_ms) + ms_var;
		timer->state = TIMER_RUNNING;
		return timer;
	}
	else
		return NULL;
}

/*
@ brief : Timer delay function, similar to pause, but unpausing automatically after defined
@ 			amount of time (delay_ms parameter). Briefly - add defined time to end value of the timer.
@ param : timer struct pointer what should be delayed
@ retval : timer struct pointer what was delayed or NULL if timer is not counting.
*/
timer_t * timer_delay(timer_t * timer, uint16_t delay_ms)
{
	if(timer->state == TIMER_RUNNING)
	{
		timer->end_val_ms = ms_var + delay_ms;
		return timer;
	}
	else
		return NULL;
}

/*
@ brief : initialize system clock on 72Mhz using external oscillator 8Mhz
@ param : none
@ retval : none
*/
void ClockInit72MhzHSE()
{
	RCC_DeInit();

	RCC->CR |= RCC_CR_HSEON;											//enable external clock (8 Mhz)
	while (!(RCC->CR & RCC_CR_HSERDY)) {/*HSE error handler code*/}; 	//wait for clock rdy

	RCC->CFGR |= RCC_CFGR_HPRE_DIV1  |									//AHB prescaler = 1
	             RCC_CFGR_PPRE2_DIV1 |									//AHB2 = sysclk
	             RCC_CFGR_PPRE1_DIV2;									//AHB1 = sysclk \ 2 (AHB1 36Mhz MAX)

	RCC->CFGR &= ~RCC_CFGR_PLLMULL;
	RCC->CFGR |= 	RCC_CFGR_PLLSRC_HSE 	|       					//PLL source = HSE (8 Mhz)
					RCC_CFGR_PLLXTPRE_HSE	|							//PLL Prescaler = 1
					RCC_CFGR_PLLMULL9;									//8 * 9 = 72 MHz

	RCC->CR |= RCC_CR_PLLON;                      						//enable PLL
	while((RCC->CR & RCC_CR_PLLRDY) == 0) {/*status code*/} 			//wait till PLL is ready

	RCC->CFGR &= ~RCC_CFGR_SW;											//clear SW bits
	RCC->CFGR |= RCC_CFGR_SW_PLL;               						//select PLL as system clock
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1)					//wait till PLL is used
	{/*PLL error handler code*/}

}

uint32_t get_core_ticks(void)
{
	return (*DWT_CYCCNT);
}
