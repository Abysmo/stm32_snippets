#ifndef _USART
#define _USART

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "string.h" 		//memset() (could be deleted for space saving)

#define UART_RX_BUFF 128

/* @brief: function for controlling min and max encoder values and loop values after overflow
 * @param: none
 * @retval: none
 */
void usart_init(void);
void send_byte(uint8_t byte);
void send_string(char * string);
void USART3_IRQHandler(void);

char uart_RX_buffer[UART_RX_BUFF];
uint16_t uart_RX_buffer_pos;

#endif
