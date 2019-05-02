#include "usart.h"

void usart_init(void)
{

	memset(&uart_RX_buffer, 0, sizeof(uart_RX_buffer));	//clear buff
	uart_RX_buffer_pos = 0;

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC->APB1ENR |= RCC_APB1Periph_USART3;
	RCC->APB2ENR |=  RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB;

	//RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//TX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// USART - 9600-8-1-N
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	//interrupt
	NVIC_SetPriority(USART3_IRQn, 0);
	NVIC_EnableIRQ(USART3_IRQn);
	USART3->CR1 |= USART_CR1_RXNEIE;	//enble usart interrupt
	USART3->CR1 |= USART_CR1_UE; 		//enabling USART
}

//uart RX handler
void USART3_IRQHandler(void)
{
	if (!(USART3->SR & USART_SR_RXNE)){return;} //if data register is empty exit from interrupt
	if ((uart_RX_buffer_pos + 1) >= UART_RX_BUFF)
	{
		/*buffer overflow code here*/

		USART3->SR &= ~USART_SR_RXNE;
		return;
	}
	uart_RX_buffer[uart_RX_buffer_pos] = USART3->DR;
	if (uart_RX_buffer[uart_RX_buffer_pos] == '\n')
	{
		/*end of incoming transmittion code here*/
	}
	uart_RX_buffer_pos++;
}


void send_byte(uint8_t byte)
{
	while (!(USART3->SR & USART_SR_TC));
	USART3->DR = byte;
}

void send_string(char * string)
{
	while (*string)
	{
		send_byte(*string++);
	}
}
