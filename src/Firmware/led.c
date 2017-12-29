#include "led.h"

volatile uint8_t ledBlinkState = 0;

static void handleLed(uint8_t led, uint16_t ledPin);

void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    GPIO_InitTypeDef gpioLed;
    
    GPIO_StructInit(&gpioLed);
    gpioLed.GPIO_Pin = LED_TX_PIN | LED_RX_PIN | LED_ERR_PIN;
    gpioLed.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioLed.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_PORT, &gpioLed);
    
    TIM_TimeBaseInitTypeDef ledBlinkTimer;
    ledBlinkTimer.TIM_Period = 59999;
    ledBlinkTimer.TIM_Prescaler = 23;
    ledBlinkTimer.TIM_ClockDivision = 0;
    ledBlinkTimer.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &ledBlinkTimer);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef blinkTimerInterrupt;
    blinkTimerInterrupt.NVIC_IRQChannel = TIM3_IRQn;
    blinkTimerInterrupt.NVIC_IRQChannelSubPriority = 0;
    blinkTimerInterrupt.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&blinkTimerInterrupt);

    // Enable the timer
    TIM_Cmd(TIM3, ENABLE);
}

void LED_Blink(uint8_t ledId)
{
   ledBlinkState |= ledId;
}

void handleLed(uint8_t led, uint16_t ledPin)
{
    if(ledBlinkState & led)
    {
        GPIO_SetBits(LED_PORT, ledPin);
        ledBlinkState &= ~led;
    }
    else
    {
        GPIO_ResetBits(LED_PORT, ledPin);
    }
}

//Timer 20ms
void TIM3_IRQHandler()
{
    handleLed(LED_TX, LED_TX_PIN);
    handleLed(LED_RX, LED_RX_PIN);
    handleLed(LED_ERR, LED_ERR_PIN);
    
    TIM3->SR &= ~(TIM_SR_UIF);
}


