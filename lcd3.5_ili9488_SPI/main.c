
#include "stm32f10x.h"
#include "timer_ctl.h"
#include "LCD_ili9488.h"


int main(void)
{
	ClockInit72MhzHSE();
	timer_init();
	LCD_FullInit();



	lcd_fill(WHITE);

	draw_rectangle(10,10, 50, 50, RED);
	draw_rectangle(130,10, 50, 50, GREEN);
	draw_rectangle(250,10, 50, 50, BLUE);


	lcd_puts("1234567890!@#$%^&*()[]_+\\|/'", 10, 100, BLACK, WHITE, &font_ubuntu18);
	lcd_puts("QWERTYUIOP{}gj[:]", 10, 200, GREEN, WHITE, &font_ubuntu26);
	lcd_puts("Zg{}_M<>?~[]", 10, 300, BLUE, WHITE, &font_ubuntu36);


	while (1)
	{

	}
}
