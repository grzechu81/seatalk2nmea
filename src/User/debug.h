#ifndef _DEBUG_H_
#define _DEBUG_H_
#include "stm32f10x.h"

void DBG_Init(void);
void DBG_Send(char* str);

#endif //_DEBUG_H_
