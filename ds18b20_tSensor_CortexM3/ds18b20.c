#include "ds18b20.h"


void delay_us(uint16_t us)
{
	volatile uint32_t core_ticks_fin = (*DWT_CYCCNT) + (us * 72);
	while ((core_ticks_fin - (*DWT_CYCCNT)) < (us * 72));
}


static uint8_t ds18b20_reset_seq(void)
{
	transmitter_mode();		// set the pin as output
	DATA_PIN_LOW;			// pull the pin low
	delay_us(480);			// reset pulse
	receiver_mode();		// set the pin as input
	delay_us(80);			// delay before slave presence pulse (60-240 us)

	if (!DATA_PIN_STATE)	// if the pin is low i.e the presence pulse is there
	{
		delay_us(420);		// wait for 400 us (MASTER Rx 480us MINIMUM)
		return 0;
	}
	else
	{
		delay_us(420);		// wait for 400 us
		return 1;
	}
}

uint8_t onewire_read (void)
{
	uint8_t value = 0;

	for (int i=0;i<8;i++)
	{
		transmitter_mode();		// set as output

		DATA_PIN_LOW;  // pull the data pin LOW
		delay_us(2);  // recovery time should be > 1 us

		receiver_mode();
		if (DATA_PIN_STATE)  // if the pin is HIGH
		{
			value |= 1<<i;  // read 1
		}
		delay_us(65);  // read time slot ( should be > 60 us)
	}
	return value;
}

void onewire_write (uint8_t data)
{
	transmitter_mode();   			// set as output
	for (int i=0; i<8; i++)
	{

		if ((data & (1<<i))!=0)		// if the bit is high
		{
			// write 1

			transmitter_mode();		// set as output
			DATA_PIN_LOW;			// pull the pin LOW
			delay_us(5);			// recovery time

			receiver_mode();		// set as input
			delay_us(80);			// wait for (60 - 120 us)
		}

		else  // if the bit is low
		{
			// write 0

			transmitter_mode();
			DATA_PIN_LOW;  // pull the pin LOW
			delay_us(80);  // wait for (60 - 120 us)
			receiver_mode();
		}
	}
}

float onewire_get_temp_c (void)
{
	if (ds18b20_reset_seq())
		return SENSOR_TIMEOUT_ERROR;
	onewire_write (0xCC);  // skip ROM
	onewire_write (0x44);  // convert t

	for (int32_t timeout_us = max_conv_time_us; timeout_us < 0; timeout_us -= 65)
	{
		transmitter_mode();		// set as output
		DATA_PIN_LOW;  // pull the data pin LOW
		delay_us(3);  // recovery time should be > 1 us
		receiver_mode();
		if (DATA_PIN_STATE)
			break;
		delay_us(65);
	}

	if (ds18b20_reset_seq())
		return SENSOR_TIMEOUT_ERROR;

	onewire_write (0xCC);  // skip ROM
	onewire_write (0xBE);  // Read Scratchpad
	uint8_t temp_l = onewire_read();
	uint8_t temp_h = onewire_read();
	uint16_t temp = (temp_h<<8)|temp_l;
	return (float)temp/16; 
}

static void receiver_mode()
{
	DATA_PORT->CRH &= ~DATA_PIN_RESET_MASK;
	DATA_PORT->CRH |= DATA_PIN_RECEIVER_MASK;
}

static void transmitter_mode()
{
	DATA_PORT->CRH &= ~DATA_PIN_RESET_MASK;
	DATA_PORT->CRH |= DATA_PIN_TRANSMITTER_MASK;
}

void ds18b20_setup(uint8_t timer_resolution)
{
	/*us timer Setup*/
	/* unlock access to DWT (ITM, etc.)registers */
	*LAR = 0xC5ACCE55;
	/* enable the use DWT */
	*DEMCR = *DEMCR | 0x01000000;
	/* Reset cycle counter */
	*DWT_CYCCNT = 0;
	/* enable cycle counter */
	*DWT_CONTROL = *DWT_CONTROL | 1 ;


	/*periph setup*/
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC->APB2ENR |= RCC_APB2_PORT;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = DATA_PIN;
	GPIO_Init(DATA_PORT, &GPIO_InitStructure);
	
	set_ds18b20_ADC_resolution(timer_resolution);

}

uint8_t set_ds18b20_ADC_resolution(uint8_t resolution)
{
	if(ds18b20_reset_seq())
		return 1;

	switch (resolution)
	{
		case SENSOR_RESOLUTION_9BIT: max_conv_time_us = 100000; break; /*max conv time 96 ms */
		case SENSOR_RESOLUTION_10BIT: max_conv_time_us = 200000; break;
		case SENSOR_RESOLUTION_11BIT: max_conv_time_us = 400000; break;
		case SENSOR_RESOLUTION_12BIT: max_conv_time_us = 800000; break;
		default: return 2;
	}

	onewire_write(0xCC);
	onewire_write(0x4E); 		/* write scratchpad */
	onewire_write(0x00);		/* first data register */
	onewire_write(0x00);		/* second data register */
	onewire_write(resolution);	/* configuration register */
	return 0;
}
