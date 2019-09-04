
#include "stm32f10x.h"
#include "timer_ctl.h"
#include "LCD_ili9486.h"


int main(void)
{
	ClockInit72MhzHSE();
	timer_init();
	LCD_FullInit();


	lcd_fill(WHITE);


	lcd_puts("1234567890!@#$%^&*()[]_+\\|/'", 10, 50, BLACK, WHITE, &font_ubuntu18);
	lcd_puts("QWERTYUIOP{}gj[:]", 10, 75, BLACK, WHITE, &font_ubuntu26);
	lcd_puts("ZXCVg{}_M<>?~[]\"@#$%", 10, 120, BLACK, WHITE, &font_ubuntu36);



	for (uint16_t i = 0; i < 480; i++)
	{
		draw_pixel(i, 260, RED);
		draw_pixel(i, 261, RED);
		draw_pixel(i, 262, RED);
		draw_pixel(i, 263, RED);
		draw_pixel(i, 264, RED);

		draw_pixel(i, 265, GREEN);
		draw_pixel(i, 266, GREEN);
		draw_pixel(i, 267, GREEN);
		draw_pixel(i, 268, GREEN);
		draw_pixel(i, 269, GREEN);

		draw_pixel(i, 270, BLUE);
		draw_pixel(i, 271, BLUE);
		draw_pixel(i, 272, BLUE);
		draw_pixel(i, 273, BLUE);
		draw_pixel(i, 274, BLUE);
	}

	while (1)
	{

	}
}
