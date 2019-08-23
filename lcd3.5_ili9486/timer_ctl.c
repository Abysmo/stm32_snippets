#include "timer_ctl.h"

inline void capture_timer(void)
{
	timer_state = 1;
}

inline void release_timer(void)
{
	timer_state = 0;
}

void timer_init(void)
{
	RCC->APB2ENR |= RCC_APB2Periph_TIM1;
}


//delay up to 43 sec
void delay_ms(uint16_t ms)
{
	TIM1->CNT = 0;
	TIM1->PSC = 48000 - 1;					// prescaler
	TIM1->ARR = (1.5 * ms);					// cycles = 1.5 ms
	TIM1->EGR |= TIM_EGR_UG;				// update generation & reset counter
	TIM1->CR1 |= TIM_CR1_ARPE;				// auto pre-load register - update after counter overflow
	TIM1->CR1 |= TIM_CR1_OPM | TIM_CR1_CEN;	// Enable timer in one time run mode
	while(TIM1->CR1 & (1 << 0)){}			// wait untill timer counting
}

//delay up to 43 sec
void timeout_ms(uint16_t ms)
{
	TIM1->CNT = 0;
	TIM1->PSC = 48000 - 1;						// prescaler
	TIM1->ARR = (1.5 * ms);						// cycles = 1.5 ms
	TIM1->EGR |= TIM_EGR_UG;					// update generation & reset counter
	TIM1->CR1 |= TIM_CR1_OPM | TIM_CR1_CEN;		// Enable timer in one time run mode
}


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
