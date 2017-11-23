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
    uint16_t depthFeet;
    uint8_t  depthFeetFr;
    uint16_t depthMeters;
    uint8_t  depthMetersFr;
} depth_t;

typedef struct
{
    uint8_t speed;
    uint8_t speedFr;
} speed_t;

void NMEA_Init(void);
void NMEA_ProcessData(uint8_t* buffer, uint8_t length);
void NMEA_SendMWV(wind_t* w);
void NMEA_SendVHW(speed_t* s);
void NMEA_SendDBT(depth_t* d);


#endif
