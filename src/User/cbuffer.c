#include "cbuffer.h"

uint8_t CB_Write(CircularBuffer_t* cb, uint16_t value)
{
    uint8_t next = cb->head + 1u;
    if(next >= cb->size)
        next = 0u;
		
    if(next == cb->tail)
        return CB_BUFFER_FULL;
		
    cb->buffer[cb->head] = value;
    cb->head = next;
    return CB_SUCCESS;
}

uint8_t CB_Read(CircularBuffer_t* cb, uint16_t* value)
{
    if(cb->head == cb->tail)
        return CB_BUFFER_EMPTY;
		
    uint8_t next = cb->tail + 1u;
    if(next >= cb->size)
        next = 0u;
		
    *value = *(cb->buffer + cb->tail);
    cb->tail = next;
    return CB_SUCCESS;
}

uint8_t CB_Capacity(CircularBuffer_t* cb)
{
    if(cb->head == cb->tail)
    {
        return cb->size - 1;
    }
    else
    {
        if(cb->head > cb->tail)
            return cb->size - (cb->head - cb->tail) - 1;
        else
            return (cb->tail - cb->head) - 1;
    }
}
