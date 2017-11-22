#include "nmea.h"
#include "led.h"
#include <string.h>
#include <stdio.h>

#define UART_TX_BUF_LEN 40

#define CR 0x0D
#define LF 0x0A

char txBuffer[UART_TX_BUF_LEN];
uint8_t txBufferLength;
uint8_t txBufferPos;

uint8_t calculateCRC(void);
uint8_t stringLength(void);
void sendStringOverUart(void);
void sendChar(char c);
char toHex(uint8_t val);

void NMEA_Init(void)
{
    txBufferLength = 0;
    txBufferPos = 0;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_InitTypeDef nmeaUsart;
    GPIO_InitTypeDef nmeaUsartTxPin;
    NVIC_InitTypeDef nmeaNVIC;

    GPIO_StructInit(&nmeaUsartTxPin);
    nmeaUsartTxPin.GPIO_Pin = GPIO_Pin_2;
    nmeaUsartTxPin.GPIO_Speed = GPIO_Speed_50MHz;
    nmeaUsartTxPin.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &nmeaUsartTxPin);

    USART_StructInit(&nmeaUsart);
    nmeaUsart.USART_BaudRate = 4800;
    nmeaUsart.USART_WordLength = USART_WordLength_8b;
    nmeaUsart.USART_StopBits = USART_StopBits_1;
    nmeaUsart.USART_Parity = USART_Parity_No;
    nmeaUsart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    nmeaUsart.USART_Mode = USART_Mode_Tx;
    USART_Init(USART2, &nmeaUsart);
    
    nmeaNVIC.NVIC_IRQChannel = USART2_IRQn;
    nmeaNVIC.NVIC_IRQChannelPreemptionPriority = 0;
    nmeaNVIC.NVIC_IRQChannelSubPriority = 0;
    nmeaNVIC.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nmeaNVIC);
    
    USART_Cmd(USART2,ENABLE);
}

// $INMWV,180.1,T,30.0,K,A*55<CR><LF>
void NMEA_SendMWV(wind_t *wind)
{
    uint8_t crc = 0;
    uint8_t strLen = 0;
    memset(txBuffer, 0, 40);

    sprintf(txBuffer, "$INMWV,%d.%d,T,%d.%d,K,A*", wind->windDir, 
        wind->windDirFr, 
        wind->windSpeed, 
        wind->windSpeedFr);

    crc = calculateCRC();
    strLen = stringLength();

    txBuffer[strLen] = toHex(crc >> 8);
    txBuffer[strLen++] = toHex(crc & 0x0f);
    txBuffer[strLen++] = CR;
    txBuffer[strLen++] = LF;
    
    txBufferLength = strLen;

    sendStringOverUart();
}

// $INVHW,x.x,T,x.x,M,x.x,N,x.x,K*hh<CR><LF>
void NMEA_SendVHW(uint16_t speedKn)
{
    uint8_t crc = 0;
    uint8_t strLen = 0;

    memset(txBuffer, 0, 40);

    sprintf(txBuffer, "$INVHW,,T,,M,%d.0,N,,K*", speedKn);

    crc = calculateCRC();
    strLen = stringLength();

    txBuffer[strLen] = toHex(crc >> 8);
    txBuffer[strLen++] = toHex(crc & 0x0f);
    txBuffer[strLen++] = CR;
    txBuffer[strLen++] = LF;
    
    txBufferLength = strLen;

    sendStringOverUart();
}

//$INDBT,x.x,f,x.x,M,x.x,F*hh<CR><LF>
void NMEA_SendDBT(uint16_t depth)
{
    uint8_t crc = 0;
    uint8_t strLen = 0;

    memset(txBuffer, 0, 40);

    sprintf(txBuffer, "$INDBT,%d.0,f,,M,,F*", depth);

    crc = calculateCRC();
    strLen = stringLength();

    txBuffer[strLen] = toHex(crc >> 8);
    txBuffer[strLen++] = toHex(crc & 0x0f);
    txBuffer[strLen++] = CR;
    txBuffer[strLen++] = LF;
    
    txBufferLength = strLen;

    sendStringOverUart();
}

char toHex(uint8_t val)
{
    return (val <= 9 ? '0' + val : 'A' + (val - 10));
}

uint8_t calculateCRC(void)
{
    uint8_t i = 0, crc = 0;

    while(txBuffer[i++] != 0 || i >= UART_TX_BUF_LEN)
    {
        if(txBuffer[i] == '$')
            continue;
        
        if(txBuffer[i] == '*')
            break;
        
        crc = crc ^ txBuffer[i];
    }
    return crc;
}

uint8_t stringLength(void)
{
    uint8_t i = 0;
    while(txBuffer[i++] != 0 && i < UART_TX_BUF_LEN);
    return i;
}

void sendStringOverUart(void)
{
    LED_On();
    txBufferPos = 0;
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_TXE))
    {
        USART_SendData(USART2, txBuffer[txBufferPos++]);
        if(txBufferPos >= txBufferLength)
        {
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
            LED_Off();
        }
    }
}
