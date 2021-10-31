#ifndef __DRV_POWER_WAVE_H_
#define __DRV_POWER_WAVE_H_
#include "stm32f1xx.h"
#define SENDWAVENUMBER                       200//Ҫ������λ�������в��ε�ѹ�������ǻ�ȡ���ĵ�ѹ��������������Ϊÿ����ѹֵ������ֳ��˸߰�λ�͵ڰ�λ
#define GETWAVENUMBER                        100//��ȡ���ĵ�ѹ����

typedef struct trans
{
	int Freq;//Ƶ��
	int Amplitude;//��ֵ
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

