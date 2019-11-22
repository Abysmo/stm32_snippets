#include "sw_timer.h"

/*
@ brief :  72Mhz system clock init with external oscillator 8mhz (most popular configuration for stm32). Should called first
@ param : none
@ retval : none
*/
void timers_init(void)
{
	ClockInit72MhzHSE();
	SysTick_Config(SystemCoreClock / 1000);
	memset(timers_array,'\0', sizeof(timers_array));
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
@ brief : release unused timer from array and free slot for new timer (theoretically this function not need at all
@ 		because all params you pass when call timer_start() function).
@ param : pointer on a timer struct what should be freed
@ retval : pointer on a timer struct what is freed by this call
*/
timer_t * free_timer(timer_t * timer)
{
	memset (timer, '\0', sizeof(timer_t));
	return timer;
}

/*
@ brief : Handler function. Check what timers is finished their count and call callbacks for this timers if assigned.
@		If there is no callback - just set flag (TIMER_DONE) in (volatile uint8_t state;) field in current structure.
@		This function should be called time to time or in Idle cycle.
@ param : none
@ retval : none
*/
void timers_handler(void)
{
	for (uint8_t i = 0;i < MAX_TIMERS; i++) /*timers passthrough*/
	{
		if(timers_array[i].state == TIMER_DONE)
		{
			if (timers_array[i].fn)
			{
				timers_array[i].fn(timers_array[i].callback_param); /*use callback with assigned parameters*/
				timers_array[i].state = TIMER_IDLE;
			}
		}
	}
}

/*
@ brief : Systick handler. Increment global tick variable, set TIMER_DONE flag if counter is done.
@ param : none
@ retval : none
*/
void SysTick_Handler(void)
{
	global_tick_ms_var++;
	for (uint8_t i = 0; i < MAX_TIMERS; i++)
	{
		if (timers_array[i].state == TIMER_TICKING)
		{
			if (global_tick_ms_var >= timers_array[i].end_val_ms)
			{
				timers_array[i].state = TIMER_DONE; /*timer is done*/
			}
		}
	}
}


/*
@ brief : simple delay function, no need to create timer for use
@ param : miliseconds
@ retval : none
*/
void delay_ms(uint16_t ms)
{
	uint32_t end = global_tick_ms_var + ms;
	while(global_tick_ms_var != end);
}

/*
@ brief : Function what starting specified timer and pass callback with parameters
@ param : Milliseconds for timeout, pointer on timer struct, callback function, callback parameters.
@ retval : none
*/
void timer_start(uint16_t ms, timer_t * timer, timer_callback_t callback_function, void * callback_fn_param)
{
	timer->fn = callback_function;
	timer->callback_param = callback_fn_param;
	timer->end_val_ms = global_tick_ms_var + ms;
	timer->state = TIMER_TICKING;
}

/*
@ brief : initialize system clock on 72mhz using external oscillator 8mhz
@ param : none
@ retval : none
*/
void ClockInit72MhzHSE()
{
	RCC_DeInit();

	RCC->CR |= RCC_CR_HSEON;								//enable external clock (8 Mhz)
	while (!(RCC->CR & RCC_CR_HSERDY)) {/*status code*/}; 	//wait for clock rdy

	RCC->CFGR |= RCC_CFGR_HPRE_DIV1  |						//AHB prescaler = 1
	             RCC_CFGR_PPRE2_DIV1 |						//AHB2 = sysclk
	             RCC_CFGR_PPRE1_DIV2;						//AHB1 = sysclk \ 2 (AHB1 36Mhz MAX)

	RCC->CFGR &= ~RCC_CFGR_PLLMULL;
	RCC->CFGR |= 	RCC_CFGR_PLLSRC_HSE 	|       		//PLL source = HSE (8 Mhz)
					RCC_CFGR_PLLXTPRE_HSE	|				//PLL Prescaler = 1
					RCC_CFGR_PLLMULL9;						//8 * 9 = 72 MHz

	RCC->CR |= RCC_CR_PLLON;                      			//enable PLL
	while((RCC->CR & RCC_CR_PLLRDY) == 0) {/*status code*/} //wait till PLL is ready

	RCC->CFGR &= ~RCC_CFGR_SW;								//clear SW bits
	RCC->CFGR |= RCC_CFGR_SW_PLL;               			//select PLL as system clock
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1)		//wait till PLL is used
	{/*status code*/}

}
