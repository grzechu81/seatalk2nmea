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

wind_t wind;
depth_t depth;
speed_t speed;

uint8_t calculateCRC(void);
uint8_t stringLength(void);
void sendStringOverUart(void);
void sendChar(char c);
char toHex(uint8_t val);

void NMEA_Init(void)
{
    txBufferLength = 0;
    txBufferPos = 0;
    
    memset(&wind, 0, sizeof(wind_t));
    memset(&depth, 0, sizeof(depth_t));
    memset(&speed, 0, sizeof(speed_t));
    
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

void NMEA_ProcessData(uint8_t* buffer, uint8_t length)
{
    uint16_t temp = 0;
    
    switch(buffer[0])
    {
        case 0x10: //Apparent Wind Direction
            temp = ((buffer[2] << 8 ) | buffer[3]);
            wind.windDir = temp / 2;
            wind.windDirFr = (temp % 2 == 0) ? 0 : 5;
            NMEA_SendMWV(&wind);
            break;
        case 0x11: //Apparent Wind Speed
            wind.windSpeed = ((buffer[2] & 0x7f) << 8) | ((buffer[3] & 0x0f) / 10);
            wind.windSpeedFr = (buffer[3] & 0x0f) % 10;
            wind.speedMs = (buffer[2] & 0x80) >> 7;
            NMEA_SendMWV(&wind);
            break;
        case 0x0: //Depth Below Transducer
            temp = (buffer[3] << 8 | buffer[4]);
            depth.depthFeet = temp / 10;
            depth.depthFeetFr = temp % 10;
            temp = (temp * 3084) / 10000;
            depth.depthMeters = temp / 10;
            depth.depthMetersFr = temp % 10;
            NMEA_SendDBT(&depth);
            break;
        case 0x20: //Speed Thru Water
            temp = (buffer[2] << 8 | buffer[3]);
            speed.speed = temp / 10;
            speed.speedFr = temp % 10;
            NMEA_SendVHW(&speed);
            break;
        default:
            break;
    }
}

// $INMWV,180.1,T,30.0,K,A*55<CR><LF>
void NMEA_SendMWV(wind_t *w)
{
    uint8_t crc = 0;
    uint8_t strLen = 0;
    memset(txBuffer, 0, 40);

    sprintf(txBuffer, "$INMWV,%d.%d,T,%d.%d,%c,A*", w->windDir, 
        w->windDirFr, 
        w->windSpeed, 
        w->windSpeedFr,
        w->speedMs ? 'M' : 'K');

    crc = calculateCRC();
    strLen = stringLength();

    strLen--; //skip the null string terminator
    txBuffer[strLen++] = toHex(crc >> 4);
    txBuffer[strLen++] = toHex(crc & 0x0f);
    txBuffer[strLen++] = CR;
    txBuffer[strLen++] = LF;
    
    txBufferLength = strLen;

    sendStringOverUart();
}

// $INVHW,x.x,T,x.x,M,x.x,N,x.x,K*hh<CR><LF>
void NMEA_SendVHW(speed_t* s)
{
    uint8_t crc = 0;
    uint8_t strLen = 0;

    memset(txBuffer, 0, 40);

    sprintf(txBuffer, "$INVHW,,T,,M,%d.%d,N,,K*", s->speed, s->speedFr);

    crc = calculateCRC();
    strLen = stringLength();

    strLen--; //skip the null string terminator
    txBuffer[strLen++] = toHex(crc >> 4);
    txBuffer[strLen++] = toHex(crc & 0x0f);
    txBuffer[strLen++] = CR;
    txBuffer[strLen++] = LF;
    
    txBufferLength = strLen;

    sendStringOverUart();
}

//$INDBT,x.x,f,x.x,M,x.x,F*hh<CR><LF>
void NMEA_SendDBT(depth_t* d)
{
    uint8_t crc = 0;
    uint8_t strLen = 0;

    memset(txBuffer, 0, 40);

    sprintf(txBuffer, "$INDBT,%d.%d,f,%d.%d,M,,F*", 
        d->depthFeet, d->depthFeetFr, 
        d->depthMeters, d->depthMetersFr);

    crc = calculateCRC();
    strLen = stringLength();

    strLen--; //skip the null string terminator
    txBuffer[strLen++] = toHex(crc >> 4);
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
    uint8_t i = 0;
    uint8_t crc = 0;

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
    LED_Blink(LED_TX);
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
        }
    }
}
