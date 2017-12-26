#ifndef _LED_H_
#define _LED_H_
#include "stm32f10x.h"

#define LED_TX_PIN GPIO_Pin_12
#define LED_RX_PIN GPIO_Pin_13
#define LED_PORT GPIOB

#define LED_TX 0x01
#define LED_RX 0x02

void LED_Init(void);
void LED_Blink(uint8_t ledId);


#endif
