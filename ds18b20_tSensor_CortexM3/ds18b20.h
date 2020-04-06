#ifndef DS18B20_H
#define DS18B20_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


#define DWT_CONTROL					((volatile uint32_t *)0xE0001000)
#define DWT_CYCCNT					((volatile uint32_t *)0xE0001004)
#define DEMCR						((volatile uint32_t *)0xE000EDFC)
#define LAR						((volatile uint32_t *)0xE0001FB0)

#define SENSOR_TIMEOUT_ERROR				0xFFFFFFFF

#define SENSOR_RESOLUTION_9BIT				0x00
#define SENSOR_RESOLUTION_10BIT				0x20
#define SENSOR_RESOLUTION_11BIT				0x40
#define SENSOR_RESOLUTION_12BIT				0x60	/*power on default*/

#define RCC_APB2_PORT					RCC_APB2Periph_GPIOB
#define DATA_PORT					GPIOB
#define DATA_PIN					GPIO_Pin_9

#define DATA_PIN_HIGH					GPIOB->ODR |= GPIO_ODR_ODR9
#define DATA_PIN_LOW					GPIOB->BRR |= GPIO_BRR_BR9
#define DATA_PIN_TOGGLE					GPIOB->ODR ^= GPIO_ODR_ODR9
#define DATA_PIN_STATE					(GPIOB->IDR & GPIO_IDR_IDR9)

#define DATA_PIN_RESET_MASK				0xF0
#define DATA_PIN_RECEIVER_MASK				0x40
#define DATA_PIN_TRANSMITTER_MASK			0x30

static uint8_t ds18b20_reset_seq(void);
static void receiver_mode(void);
static void transmitter_mode(void);
uint8_t onewire_read (void);
void onewire_write (uint8_t data);
//static void delay_us(uint16_t us);
float onewire_get_temp_c (void);
void ds18b20_setup(uint8_t sensor_resolution);
uint8_t set_ds18b20_ADC_resolution(uint8_t resolution);

static int32_t max_conv_time_us;



#endif
