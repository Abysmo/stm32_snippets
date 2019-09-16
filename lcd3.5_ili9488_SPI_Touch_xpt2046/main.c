
#include "stm32f10x.h"
#include "timer_ctl.h"
#include "LCD_ili9488.h"
#include "touch_xpt2046.h"
#include "stdlib.h"


int main(void)
{

	ClockInit72MhzHSE();
	timer_init();
	LCD_FullInit();
	Touch_FullInit();


	touch_param_t t1 = {0};

//	uint16_t x_val, y_val;
//	char str_X[10] = {0};
//	char str_Y[10] = {0};


//	draw_rectangle(10,10, 50, 50, RED);
//	draw_rectangle(130,10, 50, 50, GREEN);
//	draw_rectangle(250,10, 50, 50, BLUE);
//

//	lcd_puts("1234567890!@#$%^&*()[]_+\\|/'", 10, 100, BLACK, WHITE, &font_ubuntu18);
//	lcd_puts("QWERTYUIOP{}gj[:]", 10, 200, GREEN, WHITE, &font_ubuntu26);
//	lcd_puts("Zg{}_M<>?~[]", 10, 300, BLUE, WHITE, &font_ubuntu36);

//	NVIC_DisableIRQ(EXTI0_IRQn);
	while (1)
	{

		/*Coordinates test*/
//		while(Touch_Event)
//		{
//			read_XY(&x_val, &y_val);
//			itoa(x_val,str_X,10);
//			itoa(y_val,str_Y,10);
//
//			draw_rectangle(10,10, 100, 60, WHITE);
//
//			lcd_puts("X: ", 10, 10, RED, WHITE, &font_ubuntu18);
//			lcd_puts(str_X, 40, 10, BLACK, WHITE, &font_ubuntu18);
//			lcd_puts("Y: ", 10, 35, BLUE, WHITE, &font_ubuntu18);
//			lcd_puts(str_Y, 40, 35, BLACK, WHITE, &font_ubuntu18);
//			delay_ms(200);
//			Touch_Event = 0;
//		}

		/*Touch Test*/
		lcd_fill(WHITE);
		calibrate_touch(&t1, LCD_WIDTH, LCD_HEIGHT, 3, puts_wrapper);
		lcd_fill(WHITE);
		lcd_puts("Calibration Done!", 50, 150, BLACK, WHITE, &font_ubuntu26);
		delay_ms(2000);
		lcd_fill(WHITE);

		while(!Touch_Event)
		{
			if (get_LCD_point(&t1))
				draw_rectangle(t1.lcd_x, t1.lcd_y, 3, 3, BLACK);
			Touch_Event = 0;
		}


	}
}




