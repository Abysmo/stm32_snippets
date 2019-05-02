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


void WMU_Init();
uint32_t WMU_get_ADC_val();
double WMU_get_weight_g(int32_t w_correction);
int32_t WMU_get_correction();


#endif
