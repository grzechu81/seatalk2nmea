#ifndef _DIAG_H_
#define _DIAG_H_
#include "stm32f10x.h"

typedef struct
{
    uint8_t stMsgDropCntr;
    uint16_t stMsgRcvd;
    uint8_t stQueueCapacity;
} diag_t;

void DIAG_Init(void);
void DIAG_Send(void);

#endif //_DIAG_H_
