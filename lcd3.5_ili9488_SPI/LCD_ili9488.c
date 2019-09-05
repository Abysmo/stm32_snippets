#include "LCD_ili9488.h"

const int16_t ili9488_init_code[] =
{
	-1 ,0XF7,0xA9, 0x51, 0x2C, 0x82,
	-1, 0xC0,0x11,0x09,
	-1, 0xC1,0x41,
	-1, 0XC5,0x00,0x0A,0x80,
	-1, 0xB1,0xB0,0x11,
	-1, 0xB4,0x02,
	-1, 0xB6,0x02,0x42,
	-1, 0xB7,0xC6,
	-1, 0xBE,0x00,0x04,
	-1, 0xE9,0x00,
#ifdef SCREEN_PORTRAIT	/* Portrait memory setting */
	-1, 0x36,0x08,
#else					/* Album memory setting */
	-1, 0x36,0x68,
#endif
	-1, 0x3A,0x66,
	-1, 0xE0,0x00,0x07,0x10,0x09,
		0x17,0x0B,0x41,0x89,0x4B,
		0x0A,0x0C,0x0E,0x18,0x1B,0x0F,
	-1, 0xE1,0x00,0x17,0x1A,0x04,
		0x0E,0x06,0x2F,0x45,0x43,
		0x02,0x0A,0x09,0x32,0x36,0x0F,
	-1, 0x11,
	-2, 250,
	-1, 0x29,
	-2, 150,
	-3
};


void SPI_TXRX_Byte(uint8_t byte)
{
	while((SPI_X->SR & SPI_SR_BSY) && (!(SPI_X->SR & SPI_SR_TXE)));
	SPI_X->DR = byte;

//	while(!(SPI_X->SR & SPI_SR_RXNE));
//	return SPI_X->DR;
}

void Send_REG(uint8_t reg)
{
	LCD_RS_LOW;								/* Register select to LOW state (Register) */
	LCD_CE_LOW;
	SPI_TXRX_Byte(reg);
	LCD_CE_HIGH;
}

void Send_DATA_8(uint8_t data)
{
	LCD_RS_HIGH;							/* Register select to HIGH state (data) */
	LCD_CE_LOW;
	SPI_TXRX_Byte(data);
	LCD_CE_HIGH;
}

void Send_DATA_16(uint16_t data)
{
	LCD_RS_HIGH;	/* Register select to HIGH state (data) */
	LCD_CE_LOW;
	SPI_TXRX_Byte(data >> 8);
	SPI_TXRX_Byte((uint8_t)data);
	LCD_CE_HIGH;
}

void Send_COLOR(uint16_t color)
{
	LCD_RS_HIGH;	/* Register select to HIGH state (data) */
	LCD_CE_LOW;

	//18Bit
	SPI_TXRX_Byte((color>>8)&0xF8);	//RED
	SPI_TXRX_Byte((color>>3)&0xFC);	//GREEN
	SPI_TXRX_Byte(color<<3);		//BLUE
	LCD_CE_HIGH;
}

void LCD_FullInit(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	CTL_GPIO_CLOCKING;
	SPI_PORT_CLOCKING;
	SPI_CLOCKING;


	/* CTL GPIO config */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_RST | GPIO_RS | GPIO_CE;
	GPIO_Init(CTL_PORT, &GPIO_InitStructure);
	GPIO_SetBits(CTL_PORT, GPIO_RST | GPIO_RS | GPIO_CE);

	/* SPI GPIO config */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI | SPI_CLK | SPI_MISO;
	GPIO_Init(SPI_PORT, &GPIO_InitStructure);

	/* SPI config */
	SPI_I2S_DeInit(SPI_X);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;

	SPI_Init(SPI_X, &SPI_InitStructure);
	/* Enable SPI */
	SPI_Cmd(SPI_X, ENABLE);

	init_LCD_ili9488(ili9488_init_code);

}

void init_LCD_ili9488(const int16_t code[])
{
	int16_t const *cmd = code;
	uint8_t is_cmd = 0;
	uint8_t done = 0;

	/* Reset LCD */
	LCD_RST_LOW;
	delay_ms(150);
	LCD_RST_HIGH;
	delay_ms(50);

	/* Write init code */
	while (!done)
	{
		switch (*cmd)
		{
		case -1:
			is_cmd = 1;
			break;
		case -2:
			cmd++;
			delay_ms(*cmd);
			break;
		case -3:
			done = 1;
			break;
		default:
			if (is_cmd)
			{
				Send_REG(*cmd);
				is_cmd = 0;
			}
			else
				Send_DATA_8(*cmd);

			break;
		}
		cmd++;
	}
}

void draw_pixel(int x, int y, uint16_t color)
{
	/* Set X coord */
	Send_REG(0x2A);
	Send_DATA_16(x);
	Send_DATA_16(x);

	/* Set Y coord */
	Send_REG(0x2B);
	Send_DATA_16(y);
	Send_DATA_16(y);

	/* Write data */
	Send_REG(0x2C);
	Send_COLOR(color);
}

/* Putchar using memory write (fast) */
void lcd_putc(unsigned char c, uint16_t x, uint16_t y, uint16_t fg_color, uint16_t bg_color, font_t * font)
{

	uint16_t data_pos = font->f_offset_table[(uint8_t)c - F_START]; /* calculate data offset in data array */
	uint32_t bytes_in_char = (font->f_offset_table[(uint8_t)(c + 1) - F_START]) - (font->f_offset_table[(uint8_t)c - F_START]); /* calculate amount bytes in character */

	/* Set width and height */
	uint16_t tmp_w = (uint16_t)font->f_width_table[F_WIDTH_POS(c)] - 1; /* decrease width on 1 (when you write in memory - width larger on 1 px) */
	uint16_t tmp_h = (uint16_t)font->f_height;

/* Checking - are char fit screen ? Ignore if not */
#ifdef SCREEN_PORTRAIT
	if ((x + tmp_w - 1 ) > LCD_WIDTH)
		return;
	if ((y + tmp_h - 1 ) > LCD_HEIGHT)
		return;
#endif

#ifdef SCREEN_ALBUM
	if ((x + tmp_w - 1 ) > LCD_HEIGHT)
		return;
	if ((y + tmp_h - 1 ) > LCD_WIDTH)
		return;
#endif

	Send_REG(0x2A);				/* send memory area width */
	Send_DATA_16(x);			/* start point low bits */
	Send_DATA_16(x + tmp_w);	/* end point high bits */

	/* Set height */
	Send_REG(0x2B);				/* send memory area height */
	Send_DATA_16(y);			/* start point low bits */
	Send_DATA_16(y + tmp_h);	/* end point high bits */

	/* Write data */
	Send_REG(0x2C);					/*Memory write command*/
	for (uint32_t i = 0; i < bytes_in_char; i++, data_pos++) /* outer cycle counts data bytes in char */
	{
		for (int8_t bit_pos = 7; bit_pos >= 0; bit_pos--)	/* inner cycle counts bits in bytes and set pixels (start from highest bit -"MSB") */
		{
			if (font->f_data_table[data_pos] & (1 << bit_pos))
				Send_COLOR(fg_color);	/* send main color if pixel set */
			else
				Send_COLOR(bg_color);	/* send background color if pixel is not set */
		}
	}
}

/* Putchar using draw pixel (slow) */
//void lcd_putc(unsigned char c, uint16_t x, uint16_t y, uint16_t fg_color, uint16_t bg_color, font_t * font)
//{
//	uint16_t data_pos = font->f_offset_table[(uint8_t)c - F_START];
////	uint32_t bytes_in_char = (font->f_offset_table[(uint8_t)(c + 1) - F_START]) - (font->f_offset_table[(uint8_t)c - F_START]);
//	uint8_t bitcount = 0;
//
//	for (uint32_t i = 0; i < font->f_height ;i++)
//	{
//		for (uint8_t j = 0; j < font->f_width_table[F_WIDTH_POS(c)]; j++)
//		{
//			if (bitcount > 7)
//			{
//				data_pos++;
//				bitcount = 0;
//			}
//
//			if ((font->f_data_table[data_pos] << bitcount) & 0x80)
//			{
//				draw_pixel(j+x, i+y, fg_color);
//				bitcount++;
//			}
//
//			else
//			{
//				draw_pixel(j+x, i+y, bg_color);
//				bitcount++;
//			}
//
//		}
//	}
//
//}

void lcd_puts(const char *str, int x, int y, uint16_t fg_color, uint16_t bg_color, font_t * font)
{
	const char *c = str;
	int x_s = x;

	while (*c != '\0')
	{
		if (*c == '\n')
		{
			y += font->f_height + 1;
			x = x_s;
		}
		else
		{
			lcd_putc(*c, x, y, fg_color, bg_color, font);
			x += font->f_width_table[F_WIDTH_POS(*c)] + 1;
		}
		c++;
	}
}

void draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
	Send_REG(0x2A);
	Send_DATA_16(x);
	Send_DATA_16(x + (width -1));

	/* Set height */
	Send_REG(0x2B);
	Send_DATA_16(y);
	Send_DATA_16(y + height);

	/* Write data */
	Send_REG(0x2C);

	for (uint32_t i = 0; i < (height * width); i++)
	{
		Send_COLOR(color);
	}
}

void lcd_fill(uint16_t color)
{
	Send_REG(0x28); /*turn off screen*/
#ifdef SCREEN_ALBUM
	/* Set X coord */
	Send_REG(0x2A);
	Send_DATA_16(0);
	Send_DATA_16(480);
	/* Set Y coord */
	Send_REG(0x2B);
	Send_DATA_16(0);
	Send_DATA_16(320);

#endif

#ifdef SCREEN_PORTRAIT
	/* Set X coord */
	Send_REG(0x2A);
	Send_DATA_16(0);
	Send_DATA_16(320);
	/* Set Y coord */
	Send_REG(0x2B);
	Send_DATA_16(0);
	Send_DATA_16(480);
#endif

	/* Write data */
	Send_REG(0x2C);
	for (uint32_t i = 0; i < (LCD_WIDTH * LCD_HEIGHT); i ++)
		Send_COLOR(color);
	Send_REG(0x29); /*turn on screen*/
}


