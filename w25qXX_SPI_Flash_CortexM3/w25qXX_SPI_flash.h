#ifndef W25QXX_SPI_FLASH_H
#define W25QXX_SPI_FLASH_H



#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_rcc.h"

#define FLASH_PAGESIZE						256

#define CMD_FLASH_GET_ID					0x90
#define CMD_FLASH_WRITE_EN 					0x06
#define CMD_FLASH_READ_EN					0x03
#define CMD_FLASH_ERASE_CHIP				0xC7
#define CMD_FLASH_PAGE_PROGRAM				0x02
#define CMD_FLASH_READ_DATA					0x03
#define CMD_FLASH_BLOCK4K_ERASE				0x20
#define CMD_FLASH_BLOCK32K_ERASE			0x52
#define CMD_FLASH_BLOCK64K_ERASE			0xD8

#define SPI_TYPEDEF							SPI2

#define SPI_PGIO_TYPEDEF					GPIOB
#define SPI_MOSI_PIN						GPIO_Pin_15
#define SPI_MISO_PIN						GPIO_Pin_14
#define SPI_SCK_PIN							GPIO_Pin_13
#define SPI_CS_PIN							GPIO_Pin_12

#define SPI_PERIPH_CLOCK					RCC->APB1ENR |= RCC_APB1Periph_SPI2
#define SPI_PORT_CLOCK						RCC->APB2ENR |= RCC_APB2Periph_GPIOB
#define SPI_AF_CLOCK						RCC->APB2ENR |= RCC_APB2Periph_AFIO


#define FLASH_CS_SWITCH(STATE)				STATE == 0 ? GPIO_ResetBits(SPI_PGIO_TYPEDEF, SPI_CS_PIN) : GPIO_SetBits(SPI_PGIO_TYPEDEF,SPI_CS_PIN)


void Flash_SPI_Setup(void);
static uint8_t SPI_TXRX_Byte(uint8_t byte);

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
