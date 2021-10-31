#ifndef __DRV_595_H_
#define __DRV_595_H_
#include "stm32f1xx.h"

#define HC595AMOUNT 8  

typedef enum 
{
    Low = 0,
    High,
} HC595_StateEnum;

typedef enum 
{
    HC595_Data_DS = 0,
    HC595_SHCP,
    HC595_STCP,
    HC595_OE,
    HC595_MR, 
} HC595_PinEnum;

void Hc595TransmitMultiByte(uint8_t *data, uint8_t len);

#endif

