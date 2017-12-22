#include "diag.h"

void DIAG_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitTypeDef diagUsart;
    GPIO_InitTypeDef diagUsartTxPin;

    GPIO_StructInit(&diagUsartTxPin);
    diagUsartTxPin.GPIO_Pin = GPIO_Pin_9;
    diagUsartTxPin.GPIO_Speed = GPIO_Speed_50MHz;
    diagUsartTxPin.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &diagUsartTxPin);

    USART_StructInit(&diagUsart);
    diagUsart.USART_BaudRate = 115200;
    diagUsart.USART_WordLength = USART_WordLength_8b;
    diagUsart.USART_StopBits = USART_StopBits_1;
    diagUsart.USART_Parity = USART_Parity_No;
    diagUsart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    diagUsart.USART_Mode = USART_Mode_Tx;

    USART_Init(USART2, &diagUsart);
    USART_Cmd(USART2,ENABLE);
}

void DIAG_Send()
{
//    uint8_t i = 0;
//    if(str != 0)
//    {
//        while(str[i] != 0x0 && i < 0xff)
//        {
//            while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
//            USART_SendData(USART1, str[i]);
//            
//            i++;
//        }
//    }
}
