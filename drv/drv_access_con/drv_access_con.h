#ifndef __DRV_ACCESS_CON_H_
#define __DRV_ACCESS_CON_H_
#include "stm32f1xx.h"

typedef enum {
    RELAY_OFF = 0,
    RELAY_ON,
    CABLE_SHIBOQI, //仅用于dds波形输出的CableSe（）函数，通道接到示波器上
    CABLE_XINHAO,  //仅用于dds波形输出的CableSe（）函数，通道接到信号发生器上
} RelayStateEnum;

void  RelaySetOut(uint8_t relay, RelayStateEnum state);
void  RelaySet(uint8_t relay, RelayStateEnum state);
void  CableSet(uint8_t cable, RelayStateEnum state);
void  RelaySetZero(void);
#endif
