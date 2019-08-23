#ifndef LCD_ILI9486_H
#define LCD_ILI9486_H


#include "Fonts_Ubuntu_18_26_36.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "timer_ctl.h"



#define GPIO_WR							GPIO_Pin_11		/*Write Enable*/
#define GPIO_RST						GPIO_Pin_10		/*Reset pin*/
#define GPIO_CE							GPIO_Pin_9		/*Chip Enable*/
#define GPIO_RS							GPIO_Pin_8		/*Register select (data/command)*/
#define CTL_PORT						GPIOA
#define CTL_GPIO_CLOCKING				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)

/*
 * Important ! If you use different pins for parallel interface
 * correct GPIO masks and Send_CMD \ Send_PARAM functions also.
 *
 * Better use straight pin sequence for simple bit mask.
 * */
#define GPIO_DB0						GPIO_Pin_0
#define GPIO_DB1						GPIO_Pin_1
#define GPIO_DB2						GPIO_Pin_2
#define GPIO_DB3						GPIO_Pin_3
#define GPIO_DB4						GPIO_Pin_4
#define GPIO_DB5						GPIO_Pin_5
#define GPIO_DB6						GPIO_Pin_6
#define GPIO_DB7						GPIO_Pin_7
#define GPIO_DB0_7						0x00FF
#define DATA_PORT_DB0_7					GPIOA
#define DATA_GPIO_CLOCKING_DB0_7		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)


#define GPIO_DB8						GPIO_Pin_5
#define GPIO_DB9						GPIO_Pin_6
#define GPIO_DB10						GPIO_Pin_7
#define GPIO_DB11						GPIO_Pin_8
#define GPIO_DB12						GPIO_Pin_9
#define GPIO_DB13						GPIO_Pin_10
#define GPIO_DB14						GPIO_Pin_11
#define GPIO_DB15						GPIO_Pin_12
#define GPIO_DB8_15						0x1FE0
#define DATA_PORT_DB8_15				GPIOB
#define DATA_GPIO_CLOCKING_DB8_15		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE)


#define LCD_CE_HIGH						CTL_PORT->BRR |= GPIO_CE
#define LCD_CE_LOW						CTL_PORT->ODR |= GPIO_CE
#define LCD_WR_HIGH						CTL_PORT->ODR |= GPIO_WR
#define LCD_WR_LOW						CTL_PORT->BRR |= GPIO_WR
#define LCD_RS_HIGH						CTL_PORT->ODR |= GPIO_RS
#define LCD_RS_LOW						CTL_PORT->BRR |= GPIO_RS
#define LCD_RST_HIGH					CTL_PORT->ODR |= GPIO_RST
#define LCD_RST_LOW						CTL_PORT->BRR |= GPIO_RST

#define LCD_WIDTH						320
#define LCD_HEIGHT						480


#define WHITE			0xFFFF
#define BLACK			0x0000
#define BLUE			0x001F
#define BRED			0XF81F
#define GRED 			0XFFE0
#define GBLUE			0X07FF
#define RED				0xF800
#define MAGENTA			0xF81F
#define GREEN			0x07E0
#define CYAN			0x7FFF
#define YELLOW			0xFFE0
#define BROWN			0XBC40
#define BRRED			0XFC07
#define GRAY			0X8430
#define DARKBLUE		0X01CF
#define LIGHTBLUE		0X7D7C
#define GRAYBLUE		0X5458
#define LIGHTGREEN		0X841F
#define LIGHTGRAY		0XEF5B
#define LGRAY			0XC618
#define LGRAYBLUE		0XA651
#define LBBLUE			0X2B12


void Send_CMD(uint8_t cmd);
void Send_PARAM(uint16_t param);
void LCD_FullInit(void);
void init_LCD_ili9486(const int16_t code[]);
void draw_pixel(int x, int y, uint16_t color);
void lcd_putc(unsigned char c, uint16_t x, uint16_t y, uint16_t fg_color, uint16_t bg_color, font_t * font);
void lcd_puts(const char *str, int x, int y, uint16_t fg_color, uint16_t bg_color, font_t * font);
void draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void lcd_fill(uint16_t color);




#endif
