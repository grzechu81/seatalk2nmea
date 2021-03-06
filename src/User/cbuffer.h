#ifndef _CBUFFER_H_
#define _CBUFFER_H_
#include "stm32f10x.h"

#define CB_SUCCESS 0
#define CB_BUFFER_FULL 1
#define CB_BUFFER_EMPTY 2

typedef struct 
{
    uint8_t read;
    uint8_t write;
    uint8_t length;
    uint16_t* buffer;
}CircularBuffer_t;

uint8_t CB_Write(CircularBuffer_t* cb, uint16_t value);
uint8_t CB_Read(CircularBuffer_t* cb, uint16_t* value);
uint8_t CB_Capacity(CircularBuffer_t* cb);

#endif
