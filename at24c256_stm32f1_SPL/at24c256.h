#ifndef AT24C256_H
#define AT24C256_H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"


#define EEPROM_AT24C256_I2C_ADDRESS			0xA0
#define EEPROM_AT24C256_PAGE_SIZE_BYETES		0x40
#define EEPROM_AT24C256_LAST_BYTE_ADDR			0x7FFF
#define EEPROM_AT24C256_LAST_PAGE_ADDR			0x7FC0


/*
 * brief : init gpio and i2c bus
 * param : none
 * retval : none
 *
 * */
void EEPROM_AT24C256_I2C_Init(void);


/*
 * brief : Read byte from EEPROM
 * param : device address !!!8 bit!!!! MSB 1 0 1 0 A1 A2 A3 0 <--- LSB should present because I2C_Send7bitAddress() function
 * 			Change present RW bit not adding it. Second param it's address where byte should be readed.
 * retval : byte readed from EEPROM
 * */
uint8_t EEPROM_AT24C256_ReadByte(uint8_t dev_address, uint16_t mem_address);


/*
 * brief : Read array of data from EEPROM
 * param : device address !!!8 bit!!!! MSB 1 0 1 0 A1 A2 A3 0 <--- LSB should present because I2C_Send7bitAddress() function
 * 			Change present RW bit not adding it. mem_addres its starting address where reading should started.
 * 			data_buffer - pointer where bytes would be copied, data_len - how much bytes should be copied.
 * 			Data buffer size should be >= data_len, or this operation will cause to stack data corruption.
 * retval : amount of readed bytes, in shorthand return value = data_len;
 *
 * */
uint8_t EEPROM_AT24C256_ReadArray(uint8_t dev_address, uint16_t mem_address, uint8_t * data_buffer, uint16_t data_len);


/*
 * brief : Writing byte to EEPROM memory.
 * param : Device address 8 bit.(description above), memory address where byte should be written. Byte what should be written.
 * retval : byte what was written or 0 if address is not valid. (exceed EEPROM_AT24C256_LAST_BYTE_ADDR)
 * */
uint8_t EEPROM_AT24C256_WriteByte(uint8_t dev_address, uint16_t mem_address, uint8_t byte);


/*
 * brief : Write page (64 bytes) to EEPROM memory.
 * param : Device address 8 bit.(description above), memory address where bytes should be written.
 * 			data buffer where bytes should be readed to writing, and data len - amount of bytes to write (can't be more than 64)
 * retval : amount of writed bytes, in shorthand return value = data_len
 * 			or 0 if page address doesn't match EEPROM page layout (0x0, 0x40, 0x80 ..etc)
 * 			Address should be multiple of EEPROM_AT24C256_PAGE_SIZE_BYETES (64 or 0x40 in hex).
 * 			Also zero returned if data_len > 64 and starting page address overlap end of EEPROM memory 0x8000
 *
 * */
uint8_t EEPROM_AT24C256_WritePage(uint8_t dev_address, uint16_t mem_address, uint8_t * data, uint8_t data_len);


#endif
