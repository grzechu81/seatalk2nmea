#ifndef _NMEA_H_
#define _NMEA_H_
#include "stm32f10x.h"

typedef struct
{
    uint16_t windDir;
    uint8_t windDirFr;
    uint8_t windSpeed;
    uint8_t windSpeedFr;
    uint8_t speedMs;
} wind_t;

typedef struct
{
    uint16_t depth;
    uint8_t  depthFr;
} depth_t;

void NMEA_Init(void);
void NMEA_SendMWV(wind_t *wind);
void NMEA_SendVHW(uint16_t speedKn);
void NMEA_SendDBT(depth_t *depth);


#endif
