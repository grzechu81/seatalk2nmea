#ifndef _COMMON_H_
#define _COMMON_H_

#define BUFFER_LEN 18

#define CB_SUCCESS 0
#define CB_BUFFER_FULL 1
#define CB_BUFFER_EMPTY 2

typedef struct {
    uint8_t buffer[BUFFER_LEN];
    uint8_t pos;
    uint8_t totalLength;
} st_buffer_t;


#endif