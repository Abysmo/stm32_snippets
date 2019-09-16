#ifndef TOUCH_XPT2046_H
#define TOUCH_XPT2046_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "timer_ctl.h"
#include "stm32f10x_spi.h"
#include "LCD_ili9488.h" /* for orientation SCREEN_ALBUM or SCREEN_PORTRAIT */


#define T_SPI_X								SPI2
#define T_SPI_PORT							GPIOB
#define T_SPI_MOSI							GPIO_Pin_15
#define T_SPI_MISO							GPIO_Pin_14
#define T_SPI_CLK							GPIO_Pin_13
#define T_SPI_PORT_CLOCKING					RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE)
#define T_SPI_CLOCKING						RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE)


#define T_GPIO_CS							GPIO_Pin_1		/*Chip Enable*/
#define T_GPIO_IRQ							GPIO_Pin_0		/*IQR Pin*/
#define T_CTL_PORT							GPIOB
#define T_CTL_GPIO_CLOCKING					RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE)
#define T_CTL_IRQ_CLOCKING					RCC->APB2ENR |= RCC_APB2Periph_AFIO;

#define T_CS_LOW							T_CTL_PORT->BRR |= T_GPIO_CS
#define T_CS_HIGH							T_CTL_PORT->ODR |= T_GPIO_CS

#ifdef SCREEN_ALBUM
	#define	CMD_RDX							0x90	/* CMD for X */
	#define	CMD_RDY							0xD0	/* CMD for Y */
#else
	#define	CMD_RDX							0xD0	/* CMD for X */
	#define	CMD_RDY							0x90	/* CMD for Y */
#endif

#define	CMD_RDZ1							0xB0	/* CMD for Z1 */
#define	CMD_RDZ2							0xC0	/* CMD for Z2 */

typedef struct
{
	uint16_t x_min;		/*min X ADC touch val*/
	uint16_t y_min;		/*min Y ADC touch val*/
	uint16_t x_max;		/*max X ADC touch val*/
	uint16_t y_max;		/*max Y ADC touch va*/
	float x_factor;		/*relation factor between ADC values X coord and LCD width*/
	float y_factor;		/*relation factor between ADC values Y coord and LCD height*/
	uint16_t t_width;	/*touch width values according min X and max X ADC values*/
	uint16_t t_height;	/*touch height values according min Y and max Y ADC values*/
	uint16_t touch_x;	/*current ADC value on X coordinate*/
	uint16_t touch_y;	/*current ADC value on Y coordinate*/
	uint16_t lcd_x;		/*converted value X touch -> X LCD*/
	uint16_t lcd_y;		/*converted value Y touch -> Y LCD*/
	uint16_t lcd_width;	/*lcd screen width*/
	uint16_t lcd_height;/*lcd screen height*/
	uint8_t calibrated;	/*set it true if touch was calibrated (structure is filled)*/

}	touch_param_t;


volatile uint8_t Touch_Event;

uint8_t T_SPI_TXRX_Byte(uint8_t byte);
void Touch_FullInit(void);
void read_XY(uint16_t * x, uint16_t * y);
uint8_t calibrate_touch (touch_param_t * touch, uint16_t screen_width, uint16_t screen_height, uint8_t samples, void (*print_string)(const char * string, uint16_t x, uint16_t y, uint16_t color));
uint8_t get_LCD_point(touch_param_t * curr_touch);


#endif
