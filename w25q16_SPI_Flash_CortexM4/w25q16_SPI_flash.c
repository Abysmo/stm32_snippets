#include "w25q16_SPI_flash.h"


void SPI_Setup(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	SPI_PERIPH_CLOCK;
	SPI_PORT_CLOCK;

	/* SPI pins */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN | SPI_MISO_PIN | SPI_SCK_PIN;
	GPIO_Init(SPI_PGIO_TYPEDEF, &GPIO_InitStructure);

	/*  SPI CS PIN */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(SPI_PGIO_TYPEDEF, &GPIO_InitStructure);

	/* SPI Periph */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 10;

	SPI_Init(SPI_TYPEDEF, &SPI_InitStructure);

	SPI_TIModeCmd(SPI1, DISABLE);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

	SPI1->CR1 |= SPI_CR1_SPE; /* enable spi */
}


void FlashEraseChip(void)
{
	FlashWriteEN();
	FLASH_CS_SWITCH(0);
    SPI_TXRX_Byte(CMD_ERASE_CHIP);
    FLASH_CS_SWITCH(1);
}


void FlashBlock4kErase(uint32_t address)
{
	FlashWriteEN();
	FLASH_CS_SWITCH(0);
	SPI_TXRX_Byte(CMD_BLOCK4K_ERASE);
	SPI_TXRX_Byte( address >> 16 );
	SPI_TXRX_Byte( address >> 8 );
	SPI_TXRX_Byte( address >> 0 );
	FLASH_CS_SWITCH(1);
}


void FlashBlock32kErase(uint32_t address)
{
	FlashWriteEN();
	FLASH_CS_SWITCH(0);
	SPI_TXRX_Byte(CMD_BLOCK32K_ERASE);
	SPI_TXRX_Byte( address >> 16 );
	SPI_TXRX_Byte( address >> 8 );
	SPI_TXRX_Byte( address >> 0 );
	FLASH_CS_SWITCH(1);
}


void FlashBlock64kErase(uint32_t address)
{
	FlashWriteEN();
	FLASH_CS_SWITCH(0);
	SPI_TXRX_Byte(CMD_BLOCK64K_ERASE);
	SPI_TXRX_Byte( address >> 16 );
	SPI_TXRX_Byte( address >> 8 );
	SPI_TXRX_Byte( address >> 0 );
	FLASH_CS_SWITCH(1);
}


void FlashGetDevManID(uint8_t * TwoByteBuffer)
{
	FLASH_CS_SWITCH(0);
    SPI_TXRX_Byte(CMD_GET_ID);
    SPI_TXRX_Byte(0x00);
    SPI_TXRX_Byte(0x00);
    SPI_TXRX_Byte(0x00);
    *TwoByteBuffer = SPI_TXRX_Byte(0xff);
    *(TwoByteBuffer+1) = SPI_TXRX_Byte(0xff);
    FLASH_CS_SWITCH(1);
}


uint8_t SPI_TXRX_Byte(uint8_t byte)
{
	while((SPI_TYPEDEF->SR & SPI_SR_BSY) && (!(SPI_TYPEDEF->SR & SPI_SR_TXE)));
	SPI_TYPEDEF->DR = byte;

	while(!(SPI_TYPEDEF->SR & SPI_SR_RXNE));
	return SPI_TYPEDEF->DR;
}


void FlashWriteEN(void)
{
	FLASH_CS_SWITCH(0);
	SPI_TXRX_Byte(CMD_WRITE_EN);
	FLASH_CS_SWITCH(1);
}


void FlashWriteData(uint32_t address, uint16_t len, uint8_t * data)
{
	FlashWriteEN();

	FLASH_CS_SWITCH(0);
	SPI_TXRX_Byte(CMD_PAGE_PROGRAM);
	SPI_TXRX_Byte( address >> 16 );
	SPI_TXRX_Byte( address >> 8 );
	SPI_TXRX_Byte( address >> 0 );

	for (uint16_t i = 0; i<len; i++)
		SPI_TXRX_Byte(data[i]);

	FLASH_CS_SWITCH(1);
}


void FlashReadData(uint32_t address, uint16_t count, uint8_t * buffer)
{
	FLASH_CS_SWITCH(0);
	SPI_TXRX_Byte(CMD_READ_DATA);
	SPI_TXRX_Byte(address >> 16);
	SPI_TXRX_Byte(address >> 8);
	SPI_TXRX_Byte(address >> 0);

	for (uint16_t i=0; i<count; i++)
		buffer[i] = SPI_TXRX_Byte( 0xFF );

	FLASH_CS_SWITCH(1);
}



