#ifndef __DRV_LED_H_
#define __DRV_LED_H_
#include "stm32f1xx.h"

typedef enum
{
    LED_OFF = 0,
    LED_ON,
    LED_TOGGLE
} LedStateEnum;

typedef enum
{
    LED_1 = 0,
} LedEnum;

void SetLedState(LedEnum led, LedStateEnum state);

#endif
