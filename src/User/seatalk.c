#include <misc.h>
#include "seatalk.h"
#include "cbuffer.h"
#include "led.h"

uint8_t rxState = RX_ID;
uint16_t cbuffer[64];

CircularBuffer_t st_cbuffer = {
    .buffer = cbuffer,
    .read = 0,
    .write = 0,
    .length = 64
};

void ST_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    USART_InitTypeDef stUsart;
    GPIO_InitTypeDef stUsartRxPin;
    NVIC_InitTypeDef stNVIC;

    GPIO_StructInit(&stUsartRxPin);
    stUsartRxPin.GPIO_Pin = GPIO_Pin_11;
    stUsartRxPin.GPIO_Speed = GPIO_Speed_50MHz;
    stUsartRxPin.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &stUsartRxPin);

    USART_StructInit(&stUsart);
    stUsart.USART_BaudRate = 4800;
    stUsart.USART_WordLength = USART_WordLength_9b;
    stUsart.USART_StopBits = USART_StopBits_1;
    stUsart.USART_Parity = USART_Parity_No;
    stUsart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    stUsart.USART_Mode = USART_Mode_Rx;
    USART_Init(USART3, &stUsart);
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    stNVIC.NVIC_IRQChannel = USART3_IRQn;
    stNVIC.NVIC_IRQChannelPreemptionPriority = 0;
    stNVIC.NVIC_IRQChannelSubPriority = 0;
    stNVIC.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&stNVIC);
    USART_Cmd(USART3,ENABLE);
}

uint8_t ST_ReadData(st_buffer_t* b)
{
    uint16_t value = 0;
    uint8_t data = 0;
    uint8_t retVal = ERR_NODATA;
    
    while(CB_Read(&st_cbuffer, &value) == CB_SUCCESS)
    {
        switch(rxState)
        {
            case RX_ID:
            {
                if(value & 0x100)
                {
                    LED_On(Led_Rx);
                    data = value & 0x00ff;
                    
                    //Are we interested with this datagram ?
                    if(data == AWA_ID || 
                        data == AWS_ID || 
                        data == STW_ID || 
                        data == DBT_ID)
                    {
                        b->buffer[(b->pos)++] = data;
                        rxState = RX_LENGTH;
                    }
                }
                break;
            }
            case RX_LENGTH:
            {
                b->buffer[(b->pos)++] = (value & 0x000f);
                b->totalLength = (value & 0x000f) + MINIMAL_DATAGRAM_LENGTH;
                rxState = RX_DATA;
                break;
            }
            case RX_DATA:
            {
                if(b->pos < BUFFER_LEN)
                {
                    b->buffer[(b->pos)++] = (value & 0x00ff);
                    
                    if(b->pos >= b->totalLength)
                    {
                        rxState = RX_ID;
                        retVal = ERR_SUCCESS;
                        LED_Off(Led_Rx);
                    }
                }
                else
                {
                    rxState = RX_ID;
                    retVal = ERR_BUFFER_OVF;
                    LED_Off(Led_Rx);
                }
                break;
            }
            default:
                break;
        }

        if(retVal == ERR_SUCCESS || retVal == ERR_BUFFER_OVF)
        {
            break;
        }
    }

    return retVal;
}

uint8_t ST_IsComplete(st_buffer_t* data)
{
    uint8_t length = 0;

    if(data->pos < MINIMAL_DATAGRAM_LENGTH)
        return ERR_INCOMPLETE;

    length = (data->buffer[1] & 0x0f) + MINIMAL_DATAGRAM_LENGTH;

    if(length < data->pos)
        return ERR_INCOMPLETE;

    if((data->buffer[0] == AWA_ID) ||
        (data->buffer[0] == AWS_ID) ||
        (data->buffer[0] == DBT_ID) ||
        (data->buffer[0] == STW_ID))
    {
        return ERR_SUCCESS;
    }

    return ERR_DROP;
}

void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE))
    {
        uint16_t data = USART_ReceiveData(USART3);
        CB_Write(&st_cbuffer, data);
    }
}


