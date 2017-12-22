#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_
#include "stm32f10x.h"

void WDOG_Init(void);
void WDOG_FeedTheDog(void);

#endif
