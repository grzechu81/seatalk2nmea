#include "cbuffer.h"

static uint8_t maskWrite(CircularBuffer_t* cb)  
{ 
    return cb->write & (cb->length - 1); 
}

static uint8_t maskRead(CircularBuffer_t* cb)  
{ 
    return cb->read & (cb->length - 1); 
}

uint8_t CB_Write(CircularBuffer_t* cb, uint16_t val)  
{ 
    uint8_t retVal = CB_SUCCESS;

    if(((cb->write) - (cb->read)) < cb->length)
    {
        cb->buffer[maskWrite(cb)] = val; 
        cb->write++;
    }
    else
    {
        retVal = CB_BUFFER_FULL;
    }

    return retVal;
}

uint8_t CB_Read(CircularBuffer_t* cb, uint16_t* val)    
{ 
    uint8_t retVal = CB_SUCCESS;

    if((cb->read) != (cb->write))
    {
        *val = cb->buffer[maskRead(cb)]; 
        cb->read++;
    }
    else
    {
        retVal = CB_BUFFER_EMPTY;
    }

    return retVal;
}

uint8_t CB_Capacity(CircularBuffer_t* cb)
{ 
    return cb->length - (cb->write - cb->read); 
}
