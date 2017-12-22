#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "nmea.h"
#include "seatalk.h"
#include "diag.h"
#include "watchdog.h"
#include <string.h>

st_buffer_t rawBuffer;

void initialBlink(void);

//main function
int main(void)
{
    uint8_t result;

    LED_Init();
    DELAY_Init();
    NMEA_Init();
    ST_Init();
    WDOG_Init();
    DIAG_Init();

    memset(&rawBuffer, 0, sizeof(st_buffer_t));
    
    initialBlink();
    
    while(1)
    {
        result = ST_ReadData(&rawBuffer);
        
        if(ERR_SUCCESS == result)
        {
            result = ST_IsComplete(&rawBuffer);
            if(ERR_SUCCESS == result)
            {
                NMEA_ProcessData(rawBuffer.buffer, rawBuffer.totalLength);
                memset(&rawBuffer, 0, sizeof(st_buffer_t));
            }
        }
        
        WDOG_FeedTheDog();
    }
}

void initialBlink(void)
{
    LED_On(Led_Tx);
    LED_On(Led_Rx);
    DELAY_ms(1000);
    LED_Off(Led_Tx);
    LED_Off(Led_Rx);
}

