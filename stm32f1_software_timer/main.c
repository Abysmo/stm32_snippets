#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "sw_timer.h"

/*led blink macro*/
#define LED_ON		GPIOC->BRR |= GPIO_BRR_BR13
#define LED_OFF		GPIOC->ODR |= GPIO_ODR_ODR13


/*led blink function*/
void led_blink(void * vp)
{
	LED_ON;
	delay_ms(*((uint32_t*)vp));
	LED_OFF;
	delay_ms(*((uint32_t*)vp));
}

/*stop timer wrapper (function type casting)*/
void stop_timer_wrapper (void * vp)
{
	timer_stop((timer_t *)vp);
}

void led_gpio_init()
{
	GPIO_InitTypeDef initstruct;
	RCC->APB2ENR |= RCC_APB2Periph_GPIOC;
	initstruct.GPIO_Pin = GPIO_Pin_13;
	initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &initstruct);
}


int main(void)
{
	timers_init(); /*init timers*/
	led_gpio_init(); /*init gpio*/
	LED_OFF; /*turn off the led*/

	uint16_t var1 = 300;
	timer_t * tim1 = add_timer(); /*adding few timers*/
	timer_t * tim2 = add_timer();

	timer_start(tim1, 1000, TIMER_CYCLES_INFINITE, &led_blink, &var1); /*infinite timer, invokes led_blink function every second*/
	timer_start(tim2, 10000, 1, &stop_timer_wrapper, tim1); /*this timer shut down tim1 after 10 seconds of work*/

	while(1)
	{
		timers_handler(); /*handling timers events*/
	}

}
