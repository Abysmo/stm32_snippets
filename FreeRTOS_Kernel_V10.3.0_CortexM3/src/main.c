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
    	vTaskDelay(1000 / portTICK_PERIOD_MS);
    	GPIOC->ODR ^= GPIO_ODR_ODR13;
    }
    vTaskDelete(NULL);
}


int main(void)
{
	vFreeRTOSInitAll();
	xTaskCreate(vLedTask,"LedTask\0", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	while(1)
	{
		//shouldn't go here
	}
}
