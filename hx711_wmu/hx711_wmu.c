/*
*
*	Weight measurement unit on hx711 chip
*
*/

#include "hx711_wmu.h"


void WMU_Init()
{
	GPIO_InitTypeDef wmu_GPIO;

	//CLOCKING to periph
	RCC->APB2ENR |= WMU_PORT_CLK;		//clock to port

	// GPIOB CLK
	memset(&wmu_GPIO, 0, sizeof(wmu_GPIO));	//clear struct
	wmu_GPIO.GPIO_Pin = WMU_CLK_PIN;
	wmu_GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
	wmu_GPIO.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(WMU_PORT, &wmu_GPIO);

	// GPIOB DATA
	wmu_GPIO.GPIO_Pin = WMU_DATA_PIN;
	wmu_GPIO.GPIO_Mode = GPIO_Mode_IPU;		//GPIO_Mode_IN_FLOATING
	wmu_GPIO.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(WMU_PORT, &wmu_GPIO);

	GPIO_SetBits(WMU_PORT, WMU_CLK_PIN);	// turn off the chip
}


uint32_t WMU_get_ADC_val()
{
	uint32_t data = 0;
	GPIO_ResetBits(WMU_PORT, WMU_CLK_PIN);	// turn on chip

	while(GPIO_ReadInputDataBit(WMU_PORT, WMU_DATA_PIN)); //wait untill conversation is over

	//get serial data from 24 bit ADC
	for (uint8_t i=0; i<24; i++)
	{
		GPIO_SetBits(WMU_PORT, WMU_CLK_PIN);
		data <<= 1;		//pull shift register
		GPIO_ResetBits(WMU_PORT, WMU_CLK_PIN);
		if(GPIO_ReadInputDataBit(WMU_PORT, WMU_DATA_PIN))
			data++;		//set bit if there is data on DT pin
	}
/*
*	The output 24 bits of data is in 2’s complement
*	format. When input differential signal goes out of
*	the 24 bit range, the output data will be saturated
*	at 800000h (MIN) or 7FFFFFh (MAX), until the
*	input signal comes back to the input range.
*		- HX711 datasheet
*/
	data ^= 0x800000;
	GPIO_SetBits(WMU_PORT, WMU_CLK_PIN); // turn off the chip and set gain at 128
	return data;
}


double WMU_get_weight_g(int32_t w_correction)
{
	int32_t mid_val = 0;
	for (uint8_t i = 0; i < SAMPLES; i++)
	{
		mid_val += WMU_get_ADC_val();
	}
	mid_val /= SAMPLES;
	mid_val += w_correction;
	return WMU_LINEAR_WEIGHT(mid_val);
}


int32_t WMU_get_correction()
{
	int32_t mid_val = 0;
	for (uint8_t i = 0; i < SAMPLES; i++)
	{
		mid_val += WMU_get_ADC_val();
	}
	mid_val /= SAMPLES;

	return WMU_LINEAR_ADC_0G - mid_val;
}


