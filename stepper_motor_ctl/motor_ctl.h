#ifndef _MOTORCTL
#define _MOTORCTL

//headers
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "string.h" //memset
#include "stdlib.h" //atoi
#include "flags.h"

#define HZ_10K 			7200
#define HZ_100K 		720
#define HZ_1M 			72

#define STEP 			1.8
#define MICROSTEP 		1
#define MOTOR_ON 		GPIOA->BSRR |= GPIO_BSRR_BS11
#define MOTOR_OFF		GPIOA->BSRR |= GPIO_BSRR_BR11
#define DIR_CW 			GPIOA->BSRR |= GPIO_BSRR_BS12 	//clockwise direction
#define DIR_CCW 		GPIOA->BSRR |= GPIO_BSRR_BR12 	//counter clockwise direction

void motor_init(void);
void refresh_pwm(uint16_t freq_val);
void TIM3_IRQHandler(void);
void run_motor(uint32_t * angle);


#endif

