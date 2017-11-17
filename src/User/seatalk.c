#include "seatalk.h"

#define ST_DEBUG
uint8_t rxState = RX_ID;
uint8_t totalLength = 0;

void ST_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	
	USART_InitTypeDef stUsart;
	GPIO_InitTypeDef stUsartRxPin;
	
	GPIO_StructInit(&stUsartRxPin);
  stUsartRxPin.GPIO_Pin = GPIO_Pin_11;
  stUsartRxPin.GPIO_Speed = GPIO_Speed_50MHz;
  stUsartRxPin.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &stUsartRxPin);

#ifdef ST_DEBUG
	USART_StructInit(&stUsart);
  stUsart.USART_BaudRate = 9600;
  stUsart.USART_WordLength = USART_WordLength_8b;
  stUsart.USART_StopBits = USART_StopBits_1;
  stUsart.USART_Parity = USART_Parity_No;
  stUsart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  stUsart.USART_Mode = USART_Mode_Rx;
#else
	USART_StructInit(&stUsart);
  stUsart.USART_BaudRate = 9600;
  stUsart.USART_WordLength = USART_WordLength_9b;
  stUsart.USART_StopBits = USART_StopBits_1;
  stUsart.USART_Parity = USART_Parity_No;
  stUsart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  stUsart.USART_Mode = USART_Mode_Rx;
#endif

  USART_Init(USART3, &stUsart);
  USART_Cmd(USART3,ENABLE);
}

uint8_t ST_ReadData(st_buffer_t* buffer)
{
	uint16_t value = 0;
	uint8_t data = 0;
	uint8_t retVal = ERR_NODATA;
	
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET)
	{
		value = USART_ReceiveData(USART3);
			
		switch(rxState)
		{
			case RX_ID:
				if(value & 0x0100) //if command bit is set the value is datagram ID
				{
					data = value & 0x00ff;
					
					//Are we interested with this datagram ?
					if(data == WIND_ANGLE_ID || 
						 data == WIND_SPEED_ID || 
						 data == SPEED_THR_WTR || 
						 data == DEPTH_ID)
					{
						buffer->buffer[(buffer->pos)++] = data;
						rxState = RX_LENGTH;
						retVal = ERR_INCOMPLETE;
					}
					else
					{
						//wait for another byte with command bit set
						retVal = ERR_NODATA;
					}
				}
				break;
			case RX_LENGTH:
				buffer->buffer[(buffer->pos)++] = (value & 0x000f);
				totalLength = (value & 0x000f) + MINIMAL_DATAGRAM_LENGTH;
				rxState = RX_DATA;
				retVal = ERR_INCOMPLETE;
				break;
			case RX_DATA:
				if(totalLength < BUFFER_LEN && buffer->pos < BUFFER_LEN)
				{
					if(buffer->pos < totalLength)
					{
						buffer->buffer[(buffer->pos)++] = (value & 0x000f);
						retVal = ERR_INCOMPLETE;
					}
					else
					{
						retVal = ERR_SUCCESS;
					}
				}
				else
				{
					retVal = ERR_BUFFER_OVF;
				}
				break;
			default:
				break;
		}
	}
	
	return retVal;
}

uint8_t ST_IsComplete(st_buffer_t* buffer)
{
	uint8_t length = 0;
	
	if(buffer->pos < MINIMAL_DATAGRAM_LENGTH)
		return ERR_INCOMPLETE;
	
	length = (buffer->buffer[1] & 0x0f) + MINIMAL_DATAGRAM_LENGTH;
	
	if(length < buffer->pos)
		return ERR_INCOMPLETE;
	
	if(buffer->buffer[0] == WIND_ANGLE_ID ||
		 buffer->buffer[0] == WIND_SPEED_ID ||
	   buffer->buffer[0] == DEPTH_ID)
	{
		return ERR_SUCCESS;
	}
	
	return ERR_DROP;
}

uint8_t ST_GetDatagram(st_buffer_t* buffer)
{
	return 0;
}


