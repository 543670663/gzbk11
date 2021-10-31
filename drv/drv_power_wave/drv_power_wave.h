#ifndef __DRV_POWER_WAVE_H_
#define __DRV_POWER_WAVE_H_
#include "stm32f1xx.h"
#define SENDWAVENUMBER                       200//要传给上位机的所有波形电压个数，是获取到的电压个数的两倍，因为每个电压值都被拆分成了高八位和第八位
#define GETWAVENUMBER                        100//获取到的电压个数

typedef struct trans
{
	int Freq;//频率
	int Amplitude;//幅值
}trans1;

typedef enum {
    WAVE_IN1,
    WAVE_IN2,
    WAVE_IN3,
    WAVE_IN4,
    WAVE_IN5,
    WAVE_IN6,
    WAVE_IN7,
    WAVE_IN8,
} PowerWaveEnum;

trans1 GetPowerWaveFreq(void);
void SetPowerWave(void);
uint16_t* TransformAllWave(void);
#endif

