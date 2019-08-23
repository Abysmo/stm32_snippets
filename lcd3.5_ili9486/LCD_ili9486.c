#include "LCD_ili9486.h"


const int16_t ili9486_init_code[] = {
	// Interface Mode Control
	//-1, 0xb0, 0x0,
	// Sleep OUT
	-1, 0x11,
	-2, 250,
	// Interface Pixel Format, 16 bits / pixel
	-1, 0x3A, 0x55,
	// Memory Access Control
	-1, 0x36, 0x28,
	// Power Control 3 (For Normal Mode)
	-1, 0xC2, 0x66, //0x44
	// VCOM Control
	-1, 0xC5, 0x00, 0x00, 0x00, 0x00,
	// PGAMCTRL(Positive Gamma Control)
	-1, 0xE0, 0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08, 0x48, 0x98, 0x37, 0x0A,
		0x13, 0x04, 0x11, 0x0D, 0x00,
	// NGAMCTRL (Negative Gamma Correction)
	-1, 0xE1, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75, 0x37, 0x06,
		0x10, 0x03, 0x24, 0x20, 0x00,
	// Digital Gamma Control 1
	-1, 0xE2, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75, 0x37, 0x06,
		0x10, 0x03, 0x24, 0x20, 0x00,
	// # Sleep OUT
	-1, 0x11,
	// Display ON
	-1, 0x29,
	-2, 250,
	-3};


void Send_CMD(uint8_t cmd)
{
	LCD_RS_LOW;									/* Register select to low state (command) */

	DATA_PORT_DB0_7->BRR = GPIO_DB0_7;			/* Clear bits before setting */
	DATA_PORT_DB0_7->ODR |= cmd;				/* Setting bits */

	LCD_WR_HIGH;								/*strobe for latching data*/
	LCD_WR_LOW;									/*strobe for latching data*/
}

void Send_PARAM(uint16_t param)
{
	LCD_RS_HIGH;								/* Register select to high state (data) */

	DATA_PORT_DB0_7->BRR = GPIO_DB0_7;			/* Clear bits before setting */
	DATA_PORT_DB0_7->ODR |= (uint8_t)param;		/* Setting low bits */

	DATA_PORT_DB8_15->BRR = GPIO_DB8_15;
	param >>= 8;								/* clear less significant(low) bits */
	DATA_PORT_DB8_15->ODR |= (param << 5); 		/* shift high bits to first pin pos */

	LCD_WR_HIGH;								/* strobe for latching data */
	LCD_WR_LOW;									/* strobe for latching data */
}

void LCD_FullInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	CTL_GPIO_CLOCKING;
	DATA_GPIO_CLOCKING_DB0_7;
	DATA_GPIO_CLOCKING_DB8_15;

	/* CTL GPIO config */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_RST | GPIO_RS | GPIO_CE | GPIO_WR;
	GPIO_Init(CTL_PORT, &GPIO_InitStructure);
	GPIO_SetBits(CTL_PORT, GPIO_RST | GPIO_RS | GPIO_CE | GPIO_WR);

	/* Data GPIO config */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_DB0_7; 	/*DB0 - DB7 pins*/
	GPIO_Init(DATA_PORT_DB0_7, &GPIO_InitStructure);
	GPIO_SetBits(DATA_PORT_DB0_7, GPIO_DB0_7);

	/* Data GPIO config */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_DB8_15; 	/*DB8 - DB15 pins*/
	GPIO_Init(DATA_PORT_DB8_15, &GPIO_InitStructure);
	GPIO_SetBits(DATA_PORT_DB8_15, GPIO_DB8_15);


	/* Enable LCD Chip */
	GPIO_ResetBits(CTL_PORT, GPIO_CE);

	/* Chip Init */
	init_LCD_ili9486(ili9486_init_code);
}

void init_LCD_ili9486(const int16_t code[])
{
	int16_t const *cmd = code;
	uint8_t is_cmd = 0;
	uint8_t done = 0;

	/* Reset LCD */
	LCD_RST_LOW;
	delay_ms(250);
	LCD_RST_HIGH;

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
				Send_CMD((uint8_t) *cmd);
				is_cmd = 0;
			}
			else
				Send_PARAM((uint8_t) *cmd);

			break;
		}
		cmd++;
	}
}

void draw_pixel(int x, int y, uint16_t color)
{
	/* Set X coord */
	Send_CMD(0x2A);
	Send_PARAM(x >> 8);
	Send_PARAM(x);
	Send_PARAM(x >> 8);
	Send_PARAM(x);

	/* Set Y coord */
	Send_CMD(0x2B);
	Send_PARAM(y >> 8);
	Send_PARAM(y);
	Send_PARAM(y >> 8);
	Send_PARAM(y);

	/* Write data */
	Send_CMD(0x2C);
	Send_PARAM(color);
}

/* Putchar using memory write (fast) */
void lcd_putc(unsigned char c, uint16_t x, uint16_t y, uint16_t fg_color, uint16_t bg_color, font_t * font)
{
	uint16_t data_pos = font->f_offset_table[(uint8_t)c - F_START]; /* calculate data offset in data array */
	uint32_t bytes_in_char = (font->f_offset_table[(uint8_t)(c + 1) - F_START]) - (font->f_offset_table[(uint8_t)c - F_START]); /* calculate amount bytes in character */

	/* Set width */
	uint16_t tmp_w = (uint16_t)font->f_width_table[F_WIDTH_POS(c)] - 1; /* decrease width on 1 (when you write in memory - width larger on 1 px) */
	uint16_t tmp_h = (uint16_t)font->f_height;	/*not necessary*/

	Send_CMD(0x2A);					/* send memory area width */
	Send_PARAM(x >> 8); 			/* start point high bits */
	Send_PARAM(x);					/* start point low bits */
	Send_PARAM((x + tmp_w) >> 8); 	/* end point high bits */
	Send_PARAM(x + tmp_w);			/* end point high bits */

	/* Set height */
	Send_CMD(0x2B);					/* send memory area height */
	Send_PARAM(y >> 8);				/* start point high bits */
	Send_PARAM(y);					/* start point low bits */
	Send_PARAM((y + tmp_h) >> 8);	/* end point high bits */
	Send_PARAM(y + tmp_h);			/* end point high bits */

	/* Write data */
	Send_CMD(0x2C);					/*Memory write command*/
	for (uint32_t i = 0; i < bytes_in_char; i++, data_pos++) /* outer cycle counts data bytes in char */
	{
		for (int8_t bit_pos = 7; bit_pos >= 0; bit_pos--)	/* inner cycle counts bits in bytes and set pixels (start from highest bit -"MSB") */
		{
			if (font->f_data_table[data_pos] & (1 << bit_pos))
				Send_PARAM(fg_color);	/* send main color if pixel set */
			else
				Send_PARAM(bg_color);	/* send background color if pixel is not set */
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
	uint16_t tmp_w = width - 1;
	Send_CMD(0x2A);
	Send_PARAM(x >> 8);
	Send_PARAM(x);
	Send_PARAM((x + tmp_w) >> 8);
	Send_PARAM(x + tmp_w);

	/* Set height */
	Send_CMD(0x2B);
	Send_PARAM(y >> 8);
	Send_PARAM(y);
	Send_PARAM((y + height) >> 8);
	Send_PARAM(y + height);

	/* Write data */
	Send_CMD(0x2C);

	for (uint32_t i = 0; i < (height * width); i++)
	{
		Send_PARAM(color);
	}
}

void lcd_fill(uint16_t color)
{
	/* Set X coord */
	Send_CMD(0x2A);
	Send_PARAM(0);
	Send_PARAM(0);
	Send_PARAM(480 >> 8);
	Send_PARAM(480);

	/* Set Y coord */
	Send_CMD(0x2B);
	Send_PARAM(0);
	Send_PARAM(0);
	Send_PARAM(320 >> 8);
	Send_PARAM(320);

	/* Write data */
	Send_CMD(0x2C);
	for (uint32_t i = 0; i < (LCD_WIDTH * LCD_HEIGHT); i ++)
		Send_PARAM(color);
}


