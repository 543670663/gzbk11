#ifndef __DRV_POWER_VOL_H_
#define __DRV_POWER_VOL_H_
#include "stm32f1xx.h"
#define SENDVOLNUMBER                       80//要传给上位机的所有电压个数，是获取到的电压个数的两倍，因为每个电压值都被拆分成了高八位和第八位
#define GETVOLNUMBER                        40//获取到的电压个数
typedef enum {

    LOW_VOL1,//0
    LOW_VOL2,
    LOW_VOL3,
    LOW_VOL4,
    LOW_VOL5,
    LOW_VOL6,
    LOW_VOL7,
    LOW_VOL8,
    MID_VOL1,//8
    MID_VOL2,
    MID_VOL3,
    MID_VOL4,
    MID_VOL5,
    MID_VOL6,
    MID_VOL7,
    MID_VOL8,
    MID_VOL9,//16
    MID_VOL10,
    MID_VOL11,
    MID_VOL12,
    MID_VOL13,
    MID_VOL14,
    MID_VOL15,
    MID_VOL16,
    HI_VOL1,//24
    HI_VOL2,
    HI_VOL3,
    HI_VOL4,
    HI_VOL5,
    HI_VOL6,
    HI_VOL7,
    HI_VOL8,
    NEGVOL_TEST1,//32
    NEGVOL_TEST2,
    NEGVOL_TEST3,
    NEGVOL_TEST4,
    NEGVOL_TEST5,
    NEGVOL_TEST6,
    NEGVOL_TEST7,
    NEGVOL_TEST8,
    VOL_OFF,//40
} PowerVolEnum;


uint16_t LogPowerState(PowerVolEnum pwr_idx);//打印并且将电压值发给上位机
void SetPowerVol(PowerVolEnum pwr_idx);
uint16_t* GetAllVol(void);
#endif

