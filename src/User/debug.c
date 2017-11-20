#include "debug.h"

void DBG_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	USART_InitTypeDef debugUsart;
	GPIO_InitTypeDef debugUsartTxPin;
	
	GPIO_StructInit(&debugUsartTxPin);
  debugUsartTxPin.GPIO_Pin = GPIO_Pin_9;
  debugUsartTxPin.GPIO_Speed = GPIO_Speed_50MHz;
  debugUsartTxPin.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &debugUsartTxPin);

	USART_StructInit(&debugUsart);
  debugUsart.USART_BaudRate = 115200;
  debugUsart.USART_WordLength = USART_WordLength_8b;
  debugUsart.USART_StopBits = USART_StopBits_1;
  debugUsart.USART_Parity = USART_Parity_No;
  debugUsart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  debugUsart.USART_Mode = USART_Mode_Tx;
	
  USART_Init(USART2, &debugUsart);
  USART_Cmd(USART2,ENABLE);
}

void DBG_Send(char* str)
{
	uint8_t i = 0;
	if(str != 0)
	{
		while(str[i] != 0x0 && i < 0xff)
		{
			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
			USART_SendData(USART1, str[i]);
			
			i++;
		}
	}
}
