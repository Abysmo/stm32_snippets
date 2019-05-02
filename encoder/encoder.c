#include "encoder.h"


//button event
void EXTI0_IRQHandler(void)
{
	button_trigger ^= (1 << 0);		//switch button status
	EXTI->PR |= EXTI_PR_PR0;		//reset event
}

void encoder_ctl(void)
{
	if ((TIM1->CNT / 4) > MAX_T)
		TIM1->CNT = MAX_T * 4;
	else if ((TIM1->CNT / 4) < 4)
		TIM1->CNT = 0;
}

void encoder_init(void)
{
	__enable_irq();
	GPIO_InitTypeDef enc;

	//CLOCKING to periph
	RCC->APB2ENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1;		//GPIO + AF + Timer

	/* GPIOB Configuration: TIM1 CH1,2 (PA8,9) | PA0 - button */
	memset(&enc, 0, sizeof(enc));	//clear struct
	enc.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_0;
	enc.GPIO_Mode = GPIO_Mode_IPU;
	enc.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &enc);

	TIM1->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;		// CC1,2 channel is configured as input
	TIM1->CCER = TIM_CCER_CC1P | TIM_CCER_CC2P;				// capture compare reg enable
	TIM1->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;			// encoder mode 3
	TIM1->CR1 |= TIM_CR1_UDIS;								// disable update
	TIM1->ARR = MAX_T * 4;									// 180(maxT) * 4(one encoder click)
	TIM1->EGR |= TIM_EGR_UG;								// update timer
	TIM1->CNT = DEF_T_VAL * 4;								// set def val
	TIM1->CR1 |= TIM_CR1_CEN;								// enable timer

	// button IRQ setup
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;
	EXTI->FTSR |= EXTI_FTSR_TR0;
	EXTI->IMR |= EXTI_IMR_MR0;
	NVIC_EnableIRQ(EXTI0_IRQn);

	button_trigger = 0;
}
