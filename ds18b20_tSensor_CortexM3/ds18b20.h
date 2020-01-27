#ifndef DS18B20_H
#define DS18B20_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "sw_timer.h"

#define SENSOR_TIMEOUT_ERROR				0xFFFFFFFF

#define SENSOR_RESOLUTION_9BIT				0x00
#define SENSOR_RESOLUTION_10BIT				0x20
#define SENSOR_RESOLUTION_11BIT				0x40
#define SENSOR_RESOLUTION_12BIT				0x60	/*power on default*/

#define RCC_APB2_PORT						RCC_APB2Periph_GPIOB
#define DATA_PORT							GPIOB
#define DATA_PIN							GPIO_Pin_9

#define DATA_PIN_HIGH						GPIOB->ODR |= GPIO_ODR_ODR9
#define DATA_PIN_LOW						GPIOB->BRR |= GPIO_BRR_BR9
#define DATA_PIN_TOGGLE						GPIOB->ODR ^= GPIO_ODR_ODR9
#define DATA_PIN_STATE						(GPIOB->IDR & GPIO_IDR_IDR9)

#define DATA_PIN_RESET_MASK					0xF0
#define DATA_PIN_RECEIVER_MASK				0x40
#define DATA_PIN_TRANSMITTER_MASK			0x30

 uint8_t ds18b20_reset_seq(void);
static void receiver_mode(void);
static void transmitter_mode(void);
uint8_t onewire_read (void);
void onewire_write (uint8_t data);

float onewire_get_temp_c (void);
void ds18b20_setup(void);
uint8_t set_ADC_resolution(uint8_t resolution);

static int32_t max_conv_time_us;



#endif
