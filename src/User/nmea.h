#ifndef _NMEA_H_
#define _NMEA_H_
#include "stm32f10x.h"

typedef struct
{
	uint16_t windDir;
	uint8_t windDirFr;
	uint8_t windSpeed;
	uint8_t windSpeedFr;
} wind_t;

void NMEA_Init(void);
void NMEA_SendMWV(wind_t *wind);
void NMEA_SendVHW(uint16_t speedKn);
void NMEA_SendDBT(uint16_t depth);


#endif
