#ifndef _DIAG_H_
#define _DIAG_H_
#include "stm32f10x.h"

typedef struct
{
    uint32_t nmeaDropCounter;
    uint8_t  nmeaBufferCapacity;
    uint8_t  seatalkBufferCapacity;
} diag_t;

void DIAG_Init(void);
void DIAG_Send(void);

#endif //_DIAG_H_
