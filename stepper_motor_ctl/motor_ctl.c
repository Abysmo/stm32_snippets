#include "motor_ctl.h"


void motor_init(void)
{
	__enable_irq();

	GPIO_InitTypeDef GPIO_InitStructure;
	

	//CLOCKING to periph
	RCC->APB2ENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC;		//GPIO and AF
	RCC->APB1ENR |= RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4;														//Timers

	/* GPIOA Configuration: TIM3 CH1 (PA6) */
	memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitStructure));	//clear struct
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->CR1 |= TIM_CR1_OPM;	// one time run.
	TIM3->SMCR =  RESET;  		// Configure the external Clock source
	TIM3->SMCR |= 0x0057; 		// SMS - TRGI clock counter + Filtered Timer Input 1 (TI1FP1)
	TIM3->DIER |= TIM_DIER_UIE;	// enabling timer interrupt
	NVIC_EnableIRQ(TIM3_IRQn);	// enabling interrupt in NVIC

	//PWM TIM4 CH - PB6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //TIMER PWM
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM4->CCER |= TIM_CCER_CC1E;	// PB6 TIM4_CH1 -> output signal
	TIM4->PSC = HZ_100K;			// prescaler
	TIM4->CCMR1 |= TIM_CCMR1_OC1M;	// PWN mode 2
	TIM4->EGR |= TIM_EGR_UG;		// update generation & reset counter
	TIM4->CR1 |= TIM_CR1_CEN;		// enable pwm

	//motor control
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;		//11-on\off, 12 - direction
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	MOTOR_OFF;

}


void refresh_pwm(uint16_t freq_val)
{
	TIM4->CCR1 = freq_val / 2;		// PWM = 1/2 of period
	TIM4->ARR = freq_val;			// period
	TIM4->EGR |= TIM_EGR_UG; 		// update PWM
}


void TIM3_IRQHandler(void)
{
	state |= MOTOR_ROTN_FIN;
	MOTOR_OFF;
	TIM3->SR &= ~TIM_SR_UIF;		// resert interrupt
}


void run_motor(uint32_t * angle)
{
	TIM3->PSC = (uint16_t)MICROSTEP - 1;
	if (*angle >= 0xFFFF)
	{
		//motor_feedback();
		TIM3->ARR = (0xFFFF / STEP)-1;
		TIM3->CR1 |= TIM_CR1_UDIS;		// disable irq event generation
		TIM3->EGR |= TIM_EGR_UG; 		// update counting timer registers values (irq event)
		TIM3->CR1 &= ~TIM_CR1_UDIS;		// enable event irq event generation after registers update
		TIM3->CR1 |= TIM_CR1_CEN;		// Enable impulse counter
		MOTOR_ON;
		*angle -= 0xFFFF;
	}
	else
	{
		//motor_feedback();
		TIM3->ARR = (*angle / STEP)-1;
		TIM3->CR1 |= TIM_CR1_UDIS;		// disable irq event generation
		TIM3->EGR |= TIM_EGR_UG; 		// update counting timer registers values (irq event)
		TIM3->CR1 &= ~TIM_CR1_UDIS;		// enable event irq event generation after registers update
		TIM3->CR1 |= TIM_CR1_CEN;		// Enable impulse counter
		MOTOR_ON;
 		*angle = 0;
	}

}
