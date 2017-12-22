#include "watchdog.h"


void WDOG_Init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_4);
    IWDG_SetReload(0x0FFF);
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
    
    //Stop watchdog timer while debugging
    DBGMCU->CR |= DBGMCU_CR_DBG_IWDG_STOP;
}

void WDOG_FeedTheDog(void)
{
    IWDG_ReloadCounter();
}
