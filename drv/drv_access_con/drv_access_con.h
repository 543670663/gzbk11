#ifndef __DRV_ACCESS_CON_H_
#define __DRV_ACCESS_CON_H_
#include "stm32f1xx.h"

typedef enum {
    RELAY_OFF = 0,
    RELAY_ON,
    CABLE_SHIBOQI, //������dds���������CableSe����������ͨ���ӵ�ʾ������
    CABLE_XINHAO,  //������dds���������CableSe����������ͨ���ӵ��źŷ�������
} RelayStateEnum;

void  RelaySetOut(uint8_t relay, RelayStateEnum state);
void  RelaySet(uint8_t relay, RelayStateEnum state);
void  CableSet(uint8_t cable, RelayStateEnum state);
void  RelaySetZero(void);
#endif
