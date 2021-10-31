#ifndef __DRV_POWER_GPIO_H_
#define __DRV_POWER_GPIO_H_
#include <stdbool.h>
typedef unsigned          char uint8_t;
#define GPIOAMOUNT        8  /* GPOIPµÄ¸öÊý*/
typedef enum
{
    PB8_1_8V = 0,
    PB9_1_8V,
    PB10_3_3V,
    PB11_3_3V,
    PC1_3_3V,
    PC2_3_3V,
    PC3_5V,
    PC4_5V,
} PowerGpioEnum;

uint8_t LogPowerWorkState(void);
#endif

