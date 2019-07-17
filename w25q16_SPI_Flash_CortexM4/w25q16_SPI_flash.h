#ifndef W25Q16_SPI_FLASH
#define W25Q16_SPI_FLASH



#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"

#define FLASH_PAGESIZE						256

#define CMD_GET_ID							0x90
#define CMD_WRITE_EN 						0x06
#define CMD_READ_EN							0x03
#define CMD_ERASE_CHIP						0xC7
#define CMD_PAGE_PROGRAM					0x02
#define CMD_READ_DATA						0x03
#define CMD_BLOCK4K_ERASE					0x20
#define CMD_BLOCK32K_ERASE					0x52
#define CMD_BLOCK64K_ERASE					0xD8

#define SPI_TYPEDEF							SPI1
#define SPI_CS_PIN							GPIO_Pin_0
#define SPI_PGIO_TYPEDEF					GPIOB
#define SPI_MOSI_PIN						GPIO_Pin_5
#define SPI_MISO_PIN						GPIO_Pin_4
#define SPI_SCK_PIN							GPIO_Pin_3

#define SPI_PERIPH_CLOCK					RCC->APB2ENR |= RCC_APB2Periph_SPI1
#define SPI_PORT_CLOCK						RCC->AHB1ENR |= RCC_AHB1Periph_GPIOB


#define FLASH_CS_SWITCH(STATE)				STATE == 0 ? GPIO_ResetBits(SPI_PGIO_TYPEDEF, SPI_CS_PIN) : GPIO_SetBits(SPI_PGIO_TYPEDEF,SPI_CS_PIN)


void SPI_Setup(void);
uint8_t SPI_TXRX_Byte(uint8_t byte);

void FlashEraseChip(void);
void FlashWriteEN(void);
void FlashGetDevManID(uint8_t * TwoByteBuffer);
void FlashReadData(uint32_t address, uint16_t count, uint8_t * buffer);
void FlashWriteData(uint32_t address, uint16_t len, uint8_t * buffer);
void FlashBlock4kErase(uint32_t address);
void FlashBlock32kErase(uint32_t address);
void FlashBlock64kErase(uint32_t address);


uint8_t FlashRXBuff[FLASH_PAGESIZE];


#endif
