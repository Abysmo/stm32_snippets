#ifndef LCD_ILI9488_H
#define LCD_ILI9488_H


#include "Fonts_Ubuntu_18_26_36.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "timer_ctl.h"
#include "stm32f10x_spi.h"

/* ~~~~~~~~~ SCREEN SELECT ~~~~~~~~~*/
#define SCREEN_PORTRAIT
//#define SCREEN_ALBUM


#define SPI_X							SPI1
#define SPI_PORT						GPIOA
#define SPI_MOSI						GPIO_Pin_7
#define SPI_MISO						GPIO_Pin_6
#define SPI_CLK							GPIO_Pin_5
#define SPI_PORT_CLOCKING				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)
#define SPI_CLOCKING					RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO, ENABLE)

#define GPIO_RST						GPIO_Pin_10		/*Reset pin*/
#define GPIO_CE							GPIO_Pin_9		/*Chip Enable*/
#define GPIO_RS							GPIO_Pin_8		/*Register select (data/command)*/
#define CTL_PORT						GPIOA
#define CTL_GPIO_CLOCKING				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)

#define LCD_CE_LOW						CTL_PORT->BRR |= GPIO_CE
#define LCD_CE_HIGH						CTL_PORT->ODR |= GPIO_CE
#define LCD_RS_HIGH						CTL_PORT->ODR |= GPIO_RS
#define LCD_RS_LOW						CTL_PORT->BRR |= GPIO_RS
#define LCD_RST_HIGH					CTL_PORT->ODR |= GPIO_RST
#define LCD_RST_LOW						CTL_PORT->BRR |= GPIO_RST

#ifdef SCREEN_PORTRAIT
	#define LCD_WIDTH						320
	#define LCD_HEIGHT						480
#else
	#define LCD_WIDTH						480
	#define LCD_HEIGHT						320
#endif


#define WHITE							0xFFFF
#define BLACK							0x0000
#define BLUE							0x001F
#define BRED							0XF81F
#define GRED 							0XFFE0
#define GBLUE							0X07FF
#define RED								0xF800
#define MAGENTA							0xF81F
#define GREEN							0x07E0
#define CYAN							0x7FFF
#define YELLOW							0xFFE0
#define BROWN							0XBC40
#define BRRED							0XFC07
#define GRAY							0X8430
#define DARKBLUE						0X01CF
#define LIGHTBLUE						0X7D7C
#define GRAYBLUE						0X5458
#define LIGHTGREEN						0X841F
#define LIGHTGRAY						0XEF5B
#define LGRAY							0XC618
#define LGRAYBLUE						0XA651
#define LBBLUE							0X2B12



void Send_REG(uint8_t reg);
void Send_DATA_8(uint8_t data);
void Send_DATA_16(uint16_t data);
void Send_COLOR(uint16_t color);
void LCD_FullInit(void);
void init_LCD_ili9488(const int16_t code[]);
void draw_pixel(int x, int y, uint16_t color);
void lcd_putc(unsigned char c, uint16_t x, uint16_t y, uint16_t fg_color, uint16_t bg_color, font_t * font);
void lcd_puts(const char *str, int x, int y, uint16_t fg_color, uint16_t bg_color, font_t * font);
void draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void lcd_fill(uint16_t color);
void SPI_TXRX_Byte(uint8_t byte);


void puts_wrapper(const char *str, uint16_t x, uint16_t y, uint16_t color);


#endif
