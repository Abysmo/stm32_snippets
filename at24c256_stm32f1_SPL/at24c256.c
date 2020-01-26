#include "at24c256.h"


#define I2C_TYPEDEF				I2C1
#define I2C_GPIO_SDA				GPIO_Pin_7
#define I2C_GPIO_SCL				GPIO_Pin_6
#define I2C_PORT_TYPEDEF			GPIOB
#define CLOCK_TO_I2C				RCC->APB1ENR |= RCC_APB1Periph_I2C1
#define CLOCK_TO_I2C_PORT			RCC->APB2ENR |= RCC_APB2Periph_GPIOB



void EEPROM_AT24C256_I2C_Init(void)
{
	GPIO_InitTypeDef gpio;
	I2C_InitTypeDef i2c;

	CLOCK_TO_I2C_PORT;
	CLOCK_TO_I2C;

    i2c.I2C_ClockSpeed = 100000;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = 0xFF; //don't care
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C_TYPEDEF, &i2c);

    gpio.GPIO_Pin = I2C_GPIO_SDA | I2C_GPIO_SCL;
    gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_PORT_TYPEDEF, &gpio);

    I2C_Cmd(I2C_TYPEDEF, ENABLE);
}


uint8_t EEPROM_AT24C256_ReadByte(uint8_t dev_address, uint16_t mem_address)
{

	I2C_AcknowledgeConfig(I2C_TYPEDEF,ENABLE);

	I2C_GenerateSTART(I2C_TYPEDEF,ENABLE);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C_TYPEDEF, dev_address, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1,(mem_address >> 8));
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1,(uint8_t)(mem_address));
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C_TYPEDEF,ENABLE);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C_TYPEDEF, dev_address, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_RECEIVED));
	uint8_t readedbyte = I2C_ReceiveData(I2C_TYPEDEF);

	I2C_GenerateSTOP(I2C_TYPEDEF,ENABLE);
	I2C_AcknowledgeConfig(I2C_TYPEDEF,DISABLE);

	return readedbyte;
}

uint8_t EEPROM_AT24C256_ReadArray(uint8_t dev_address, uint16_t mem_address, uint8_t * data_buffer, uint16_t data_len)
{

	if((!(data_buffer)) || data_len <= 0)
		return 0;

	I2C_AcknowledgeConfig(I2C_TYPEDEF,ENABLE);

	I2C_GenerateSTART(I2C_TYPEDEF,ENABLE);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C_TYPEDEF, dev_address, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1,(mem_address >> 8));
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1,(uint8_t)(mem_address));
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C_TYPEDEF,ENABLE);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C_TYPEDEF, dev_address, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	for(uint16_t i = data_len; i > 0; i--, data_buffer++)
	{
		while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_RECEIVED));
		*data_buffer = I2C_ReceiveData(I2C_TYPEDEF);
	}

	I2C_GenerateSTOP(I2C_TYPEDEF,ENABLE);
	I2C_AcknowledgeConfig(I2C_TYPEDEF,DISABLE);

	return data_len;
}


uint8_t EEPROM_AT24C256_WriteByte(uint8_t dev_address, uint16_t mem_address, uint8_t byte)
{
	if(mem_address > EEPROM_AT24C256_LAST_BYTE_ADDR)
		return 0;

	I2C_GenerateSTART(I2C_TYPEDEF,ENABLE);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C_TYPEDEF, dev_address, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1,mem_address >> 8);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1,mem_address);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1,byte);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTOP(I2C_TYPEDEF,ENABLE);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	return byte;
}

uint8_t EEPROM_AT24C256_WritePage(uint8_t dev_address, uint16_t mem_address, uint8_t * data, uint8_t data_len)
{

	if((data_len > EEPROM_AT24C256_PAGE_SIZE_BYETES) || /* check pagesize */
	((mem_address) % EEPROM_AT24C256_PAGE_SIZE_BYETES) || /*page address aligned to 64bytes */
	(mem_address > EEPROM_AT24C256_LAST_PAGE_ADDR)) /*page address validation and for avoiding data alteration */
		return 0;

	I2C_GenerateSTART(I2C_TYPEDEF,ENABLE);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C_TYPEDEF, dev_address, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1,mem_address >> 8);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C1,mem_address);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	for(uint8_t i = data_len; i > 0; i--, data++)
	{
		I2C_SendData(I2C1, *data);
		while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}

	I2C_GenerateSTOP(I2C_TYPEDEF,ENABLE);
	while (!I2C_CheckEvent(I2C_TYPEDEF, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	return data_len;

}





