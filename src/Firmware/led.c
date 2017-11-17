#include "led.h"
#include "stm32f10x.h"

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef gpioLed;
	GPIO_StructInit(&gpioLed);
	
	gpioLed.GPIO_Pin = GPIO_Pin_13;
	gpioLed.GPIO_Mode = GPIO_Mode_Out_OD;
	gpioLed.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOC, &gpioLed);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

void LED_On(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void LED_Off(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

