/*
*
*	Weight measurement unit on hx711 chip
*
*/

#ifndef _HX_711_WMU_H
#define _HX_711_WMU_H

#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "string.h"

#define WMU_DATA_PIN			GPIO_Pin_12
#define WMU_CLK_PIN				GPIO_Pin_13
#define WMU_PORT				GPIOB
#define WMU_PORT_CLK			RCC_APB2Periph_GPIOB

#define SAMPLES					5

// Linear formula up to 500 gr / 2kg tenso
// f(x) = 0.0009566078x - 8017.9185049363
// R² = 0.9999987604
#define WMU_LINEAR_WEIGHT(ADC)			((0.0009566078 * ADC) - 8017.9185049363)
#define WMU_LINEAR_ADC_0G				8381577

/*
 @brief : init MCU GPIO
 @param : none
 @retval : none
*/
void WMU_Init();

/*
 @brief : get ADC value from hx711 chip using serial interface
 @param : none
 @retval : 24 bit value XOR'ed with 0x800000
*/
uint32_t WMU_get_ADC_val();

/*
 @brief : function for weight calculation in grams
 @param : none
 @retval : weight in grams
*/
double WMU_get_weight_g();

/*
 @brief : Tare procedure. Calculate difference between original adc value (25 C \ 0 g) and current,
 @ also refresh wmu_correction var. wich used in weight calculation
 @param : none
 @retval : none
*/
void WMU_tare(void);

#endif
