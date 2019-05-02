#ifndef _ENCODER 
#define _ENCODER

//headers
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "string.h"			//memset (could be deleted for space saving)

#define DEF_T_VAL	30		//default temperature value
#define MAX_T		180		//encoder max val

/* @brief: periphery setup for encoder
 * @param: none
 * @retval: none
 */
void encoder_init(void);

/* @brief: EXTI0 handler function brief
 * @param: none
 * @retval: none
 */
void EXTI0_IRQHandler(void);

/* @brief: function for controlling min and max encoder values and loop values after overflow
 * @param: none
 * @retval: none
 */
void encoder_ctl(void);



uint8_t button_trigger; 	//button status var

#endif
