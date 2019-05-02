#ifndef _ADC_TERM
#define _ADC_TERM

#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "math.h" //log


#define BALANCE_RESISTOR    10000.0		//ref resistor val in Ohm's
#define MAX_ADC             4096.0		//12 bit max value of ADC
#define BETA                3950.0		//Beta
#define ROOM_TEMP           298.15		//Room temperature in Kelvin
#define RESISTOR_ROOM_TEMP  100000.0	//Resistor room t value

/* @brief: periphery setup for ADC
 * @param: none
 * @retval: none
 */
void gpio_ADC_Init();

/* @brief: getting ADC value from HX711
 * @param: none
 * @retval: 12bit value
 */
uint16_t get_ADC_val();

/* @brief: ADC calibration
 * @param: none
 * @retval: none
 */
void calibrate_ADC();

/* @brief: get temperature in Celsius
 * @param: samples - amount of measurements
 * @retval: temperature in Celsius
 */
int get_temp_c(uint8_t samples);

#endif
