#ifndef __DRV_POWER_DDS_H_
#define __DRV_POWER_DDS_H_
#include "stm32f1xx.h"

#define DDSAMOUNT 8  /* ADG714�ջ��������ӵ�Ƭ��*/
#define DDSNAMELEN 10  /* ADG714�ջ��������ӵ�Ƭ��*/
typedef enum {
    DDS_OUT1,
    DDS_OUT2,
    DDS_OUT3,
    DDS_OUT4,
    DDS_OUT5,
    DDS_OUT6,
    DDS_OUT7,
    DDS_OUT8,
} PowerDdsEnum;

void AD9837_Init(uint16_t WaveType, float FRQ, float Phase, PowerDdsEnum pwr_idx);

#endif

