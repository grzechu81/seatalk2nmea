#include "delay.h"
#include "stm32f10x.h"

volatile uint32_t timerCounter;

void DELAY_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef delayTimer;
    delayTimer.TIM_Period = 35999;
    delayTimer.TIM_Prescaler = 2;
    delayTimer.TIM_ClockDivision = 0;
    delayTimer.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &delayTimer);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef delayTimerInterrupt;
    delayTimerInterrupt.NVIC_IRQChannel = TIM2_IRQn;
    delayTimerInterrupt.NVIC_IRQChannelSubPriority = 0;
    delayTimerInterrupt.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&delayTimerInterrupt);

    // Enable the timer
    TIM_Cmd(TIM2, ENABLE);
}

void DELAY_ms(uint32_t delay)
{
    timerCounter = delay;
    while(timerCounter > 0);
}

void TIM2_IRQHandler()
{
    if(timerCounter > 0)
        timerCounter --;
    
    TIM2->SR &= ~(TIM_SR_UIF);
}
