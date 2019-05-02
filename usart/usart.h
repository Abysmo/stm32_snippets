#ifndef _USART
#define _USART

#include "stm32f10x.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "string.h" 		//memset() (could be deleted for space saving)

#define UART_RX_BUFF 128

/* @brief: init function for usart
 * @param: none
 * @retval: none
 */
void usart_init(void);

/* @brief: send byte via usart
 * @param: byte to send
 * @retval: none
 */
void send_byte(uint8_t byte);

/* @brief: send string via usart using send_byte()
 * @param: string to send
 * @retval: none
 */
void send_string(char * string);

/* @brief: USART RX handler
 * @param: none
 * @retval: none
 */
void USART3_IRQHandler(void);

char uart_RX_buffer[UART_RX_BUFF];	// RX buffer
uint16_t uart_RX_buffer_pos;		// RX buff position

#endif
