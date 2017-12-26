#include "diag.h"
#include "common.h"
#include <string.h>
#include <stdio.h>

diag_t diagData;
char diagBuffer[50];
volatile uint8_t sendDiagDataUpdate;

static void sendString(char* str);

void DIAG_Init(void)
{
    sendDiagDataUpdate = FALSE;
    
#ifdef DEBUG
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    //UART1 Initialization
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

    USART_Init(USART1, &diagUsart);
    USART_Cmd(USART1,ENABLE);
    
    //Diag timer initialization
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseInitTypeDef diagTimer;
    diagTimer.TIM_Period = 63999;
    diagTimer.TIM_Prescaler = 1124;
    diagTimer.TIM_ClockDivision = 0;
    diagTimer.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM4, &diagTimer);

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef diagTimerInterrupt;
    diagTimerInterrupt.NVIC_IRQChannel = TIM4_IRQn;
    diagTimerInterrupt.NVIC_IRQChannelSubPriority = 0;
    diagTimerInterrupt.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&diagTimerInterrupt);

    // Enable the timer
    TIM_Cmd(TIM4, ENABLE);
#endif
}

void DIAG_Send()
{
    if(sendDiagDataUpdate)
    {
        memset(diagBuffer, '\0', 50);
        sprintf(diagBuffer, "DCTR: %d, STBC: %d, NBC: %d\r\n", diagData.nmeaDropCounter, 
                                                           diagData.seatalkBufferCapacity,
                                                           diagData.nmeaBufferCapacity);
        sendString(diagBuffer);
        sendDiagDataUpdate = FALSE;
    }
}

static void sendString(char* str)
{
    uint8_t i = 0;
    if(str != 0)
    {
        while(str[i] != 0x0 && i < 0xff)
        {
            while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 
            USART_SendData(USART1, str[i]);
            i++;
        }
    }
}

//Timer 1000ms
void TIM4_IRQHandler()
{
    sendDiagDataUpdate = TRUE;
    TIM4->SR &= ~(TIM_SR_UIF);
}
