#include "led.h"

uint16_t ledPins[2] = {LED_TX_PIN, LED_RX_PIN};

void LED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpioLed;
    
    GPIO_StructInit(&gpioLed);
    gpioLed.GPIO_Pin = LED_TX_PIN | LED_RX_PIN;
    gpioLed.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioLed.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_PORT, &gpioLed);
}

void LED_On(led_t ledId)
{
    GPIO_SetBits(LED_PORT, ledPins[ledId]);
}

void LED_Off(led_t ledId)
{
    GPIO_ResetBits(LED_PORT, ledPins[ledId]);
}

