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
depth_t depth;

//main function
int main(void)
{
    uint8_t result;

    LED_Init();
    DELAY_Init();
    NMEA_Init();
    ST_Init();

    memset(&wind, 0, sizeof(wind_t));
    memset(&depth, 0, sizeof(depth_t));
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
    
    switch(type)
    {
        case AWA_ID:
            temp = ((b->buffer[2] << 8 ) | b->buffer[3]);
            wind.windDir = temp / 2;
            wind.windDirFr = (temp % 2 == 0) ? 0 : 5;
            NMEA_SendMWV(&wind);
            break;
        case AWS_ID:
            wind.windSpeed = ((b->buffer[2] & 0x7f) << 8) | ((b->buffer[3] & 0x0f) / 10);
            wind.windSpeedFr = (b->buffer[3] & 0x0f) % 10;
            wind.speedMs = (b->buffer[2] & 0x80) >> 7;
            NMEA_SendMWV(&wind);
            break;
        case DBT_ID:
            temp = (b->buffer[3] << 8 | b->buffer[4]);
            depth.depth = temp / 10;
            depth.depthFr = temp % 10;
            NMEA_SendDBT(&depth);
            break;
        case STW_ID:
            temp = (b->buffer[2] << 8 | b->buffer[3]) / 10;
            NMEA_SendVHW(temp);
            break;
        default:
            break;
    }
}
