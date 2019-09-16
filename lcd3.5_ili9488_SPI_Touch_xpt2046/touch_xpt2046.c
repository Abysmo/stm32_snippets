#include "touch_xpt2046.h"


void Touch_FullInit(void)
{
	__enable_irq();

	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	T_CTL_GPIO_CLOCKING;
	T_SPI_PORT_CLOCKING;
	T_SPI_CLOCKING;
	T_CTL_IRQ_CLOCKING;

	/* CTL GPIO config */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = T_GPIO_CS;
	GPIO_Init(T_CTL_PORT, &GPIO_InitStructure);

	/* IRQ GPIO config */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = T_GPIO_IRQ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(T_CTL_PORT, &GPIO_InitStructure);

	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PB;	/*alternate function control register setup port B0*/
	EXTI->FTSR |= EXTI_FTSR_TR0;				/*Falling tide event*/
//	EXTI->FTSR |= EXTI_RTSR_TR0;				/*Rising tide event*/
	EXTI->IMR |= EXTI_IMR_MR0;					/*Unmask Interrupt line*/
	NVIC_EnableIRQ(EXTI0_IRQn);


	/* SPI GPIO config */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = T_SPI_MOSI | T_SPI_CLK | T_SPI_MISO;
	GPIO_Init(T_SPI_PORT, &GPIO_InitStructure);

	/* SPI config */
	SPI_I2S_DeInit(T_SPI_X);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(T_SPI_X, &SPI_InitStructure);

	/* Enable SPI */
	SPI_Cmd(T_SPI_X, ENABLE);

}

void EXTI0_IRQHandler(void)
{
	Touch_Event = 1;			//set touch event
	EXTI->PR |= EXTI_PR_PR0;	//reset event
}

uint8_t T_SPI_TXRX_Byte(uint8_t byte)
{

	while((T_SPI_X->SR & SPI_SR_BSY) && (!(T_SPI_X->SR & SPI_SR_TXE)));
	T_SPI_X->DR = byte;

	while(!(T_SPI_X->SR & SPI_SR_RXNE));
	return T_SPI_X->DR;

}

void read_XY(uint16_t * x, uint16_t * y)
{

	T_CS_LOW;

	T_SPI_TXRX_Byte(CMD_RDX); /* Get X */
	*x = T_SPI_TXRX_Byte(0);
	*x <<= 8;
	*x |= T_SPI_TXRX_Byte(0);
	*x >>= 3;

	T_SPI_TXRX_Byte(CMD_RDY); /* Get Y */
	*y = T_SPI_TXRX_Byte(0);
	*y <<= 8;
	*y |= T_SPI_TXRX_Byte(0);
	*y >>= 3;

	T_CS_HIGH;
}

uint8_t calibrate_touch (touch_param_t * touch, uint16_t screen_width, uint16_t screen_height, uint8_t samples, void (*print_string)(const char * string, uint16_t x, uint16_t y, uint16_t color))
{
	uint16_t tmp_x, sum_x = 0;
	uint16_t tmp_y, sum_y = 0;

	print_string("- CALIBRATION -", 75, 100, 0);
	print_string("Tap + with pen on top left and", 20, 130, 0);
	print_string("bottom right corners untill", 20, 150, 0);
	print_string("they disappear", 50, 170, 0);


	print_string("+", 5, 0, 0);
	for (uint8_t i = 0; i < samples; i++)
	{
		Touch_Event = 0;
		while(!Touch_Event);
		read_XY(&tmp_x, &tmp_y);
		sum_x += tmp_x;
		sum_y += tmp_y;
		delay_ms(500);
	}
	print_string("+",5, 0, 0xFFFF);
#ifdef SCREEN_ALBUM
	touch->x_min = sum_x / samples;
	touch->y_min = sum_y / samples;
#else
	touch->x_max = sum_x / samples;
	touch->y_min = sum_y / samples;
#endif


	tmp_x = 0;
	sum_x = 0;
	tmp_y = 0;
	sum_y = 0;
	print_string("+",screen_width - 15,screen_height - 25, 0);
	for (uint8_t j = 0; j < samples; j++)
	{
		Touch_Event = 0;
		while(!Touch_Event);
		read_XY(&tmp_x, &tmp_y);
		sum_x += tmp_x;
		sum_y += tmp_y;
		delay_ms(500);
	}
	print_string("+",screen_width - 15,screen_height - 25, 0xFFFF);
#ifdef SCREEN_ALBUM
	touch->x_max = sum_x / samples;
	touch->y_max = sum_y / samples;
#else
	touch->x_min = sum_x / samples;
	touch->y_max = sum_y / samples;
#endif


	touch->t_width = touch->x_max - touch->x_min;
	touch->t_height = touch->y_max - touch->y_min;

	touch->x_factor = (float)touch->t_width / (float)screen_width;
	touch->y_factor = (float)touch->t_height / (float)screen_height;

	Touch_Event = 0;
	return 0;
}

uint8_t get_LCD_point(touch_param_t * curr_touch)
{
	read_XY(&curr_touch->touch_x, &curr_touch->touch_y);
	if ((!curr_touch->touch_x) || (!curr_touch->touch_y))
		return 0;
#ifdef SCREEN_ALBUM
	curr_touch->lcd_x = (curr_touch->touch_x - curr_touch->x_min) / curr_touch->x_factor;
#else
	curr_touch->lcd_x = (curr_touch->x_max - curr_touch->touch_x) / curr_touch->x_factor;
#endif
	curr_touch->lcd_y = (curr_touch->touch_y - curr_touch->y_min) / curr_touch->y_factor;
	return 1;
}





