#ifndef _LED_H_
#define _LED_H_
#include "stm32f10x.h"

#define LED_TX_PIN GPIO_Pin_12
#define LED_RX_PIN GPIO_Pin_13
#define LED_PORT GPIOB

typedef enum 
{
    Led_Tx,
    Led_Rx
}led_t;

void LED_Init(void);
void LED_On(led_t led);
void LED_Off(led_t led);


#endif
