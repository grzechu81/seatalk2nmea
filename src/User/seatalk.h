#ifndef _SEATALK_H
#define _SEATALK_H
#include "stm32f10x.h"

#define BUFFER_LEN 10
#define MINIMAL_DATAGRAM_LENGTH 3

#define SPEED_THR_WTR 20
#define WIND_ANGLE_ID 10
#define WIND_SPEED_ID 11
#define DEPTH_ID      0

#define ERR_SUCCESS    0
#define ERR_BUFFER_OVF 1
#define ERR_INCOMPLETE 2
#define ERR_DROP       3
#define ERR_NODATA     4

#define RX_ID     0 
#define RX_LENGTH 1
#define RX_DATA   2

typedef struct {
	uint8_t buffer[BUFFER_LEN];
	uint8_t pos;
} st_buffer_t;

void ST_Init(void);
uint8_t ST_ReadData(st_buffer_t* buffer);
uint8_t ST_IsComplete(st_buffer_t* buffer);
uint8_t ST_GetDatagram(st_buffer_t* buffer);



#endif
