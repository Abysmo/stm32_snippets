#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "list.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


void vFreeRTOSInitAll()
{
	GPIO_InitTypeDef port;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_StructInit(&port);
    port.GPIO_Mode = GPIO_Mode_Out_PP;
    port.GPIO_Pin = GPIO_Pin_13;
    port.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &port);
}

void vLedTask (void *pvParameters)
{
    while(1)
    {
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
		for(int i = 0; i<500000;i++){};
    }
    vTaskDelete(NULL);
}

void vLedTask2 (void *pvParameters)
{
    while(1)
    {
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);
		for(int i = 0; i<100000;i++){};
    }
    vTaskDelete(NULL);
}

int main(void)
{
	vFreeRTOSInitAll();
	xTaskCreate(vLedTask,(const char*)"LedTask\0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(vLedTask2,(const char*)"LedTask2\0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	while(1)
	{
		//shouldn't go here
	}
}
