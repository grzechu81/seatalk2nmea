#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "nmea.h"
#include "seatalk.h"
#include "debug.h"
#include <string.h>

void sendNmeaData(st_buffer_t* b);

st_buffer_t rawBuffer;
wind_t wind;

//main function
int main(void)
{
    uint8_t result;

    LED_Init();
    DELAY_Init();
    NMEA_Init();
    ST_Init();

    memset(&rawBuffer, 0, sizeof(st_buffer_t));
    while(1)
    {
        result = ST_ReadData(&rawBuffer);
        
        if(ERR_SUCCESS == result)
        {
            result = ST_IsComplete(&rawBuffer);
            if(ERR_SUCCESS == result)
            {
                //LED_On();
                sendNmeaData(&rawBuffer);
                memset(&rawBuffer, 0, sizeof(st_buffer_t));
                //LED_Off();
            }
        }
    }
}

void sendNmeaData(st_buffer_t* b)
{
    uint8_t type = b->buffer[0];
    uint16_t temp = 0;
    memset(&wind, 0, sizeof(wind));

    switch(type)
    {
        case AWA_ID:
            temp = ((b->buffer[2] << 8 ) | b->buffer[3]) / 2;
            wind.windDir = temp;
            NMEA_SendMWV(&wind);
            break;
        case AWS_ID:
            temp = (b->buffer[2] & 0x7f) + ((b->buffer[3] & 0x0f) / 10);
            wind.windSpeed = temp;
            NMEA_SendMWV(&wind);
            break;
        case DBT_ID:
            temp = (b->buffer[3] << 8 | b->buffer[4]) / 10;
            NMEA_SendDBT(temp);
            break;
        case STW_ID:
            temp = (b->buffer[2] << 8 | b->buffer[3]) / 10;
            NMEA_SendVHW(temp);
            break;
        default:
            break;
    }
}
