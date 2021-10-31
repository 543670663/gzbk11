#include "./drv_power_wave/drv_power_wave.h"
#include "drv_power_wave.h"
#include "drv_log.h"
#include "adc.h"
#include "drv_adg714.h"
#include <stdio.h>

char *WAVENAME[] = {
    "WAVE_IN1", "WAVE_IN2",  "WAVE_IN3",  "WAVE_IN4",  "WAVE_IN5",
    "WAVE_IN6",  "WAVE_IN7",  "WAVE_IN8"
};

uint16_t ADC_Value[GETWAVENUMBER];
uint16_t ONEWAVE[SENDWAVENUMBER];

void SetPowerWave()//�������򿪿��� ת��adcȡ��ֵ
{
    HAL_ADCEx_Calibration_Start(&hadc3);//�������
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)ADC_Value, sizeof(ADC_Value) / sizeof(ADC_Value[0])); //��dma���βɼ�
    while ((DMA2->ISR & 0x20000) == 0);
    HAL_ADC_Stop_DMA(&hadc3);//�ر�dma���βɼ�

    for (uint32_t i = 0; i < sizeof(ADC_Value) / sizeof(ADC_Value[0]); i++) { //������ĵ�ѹֵ��ӡ
        ADC_Value[i] =  ADC_Value[i] * 3.3 / 4096 * 1000;
        LOG_PRINTF("%02dmv", ADC_Value[i]);
    }
}

trans1 GetPowerWaveFreq(void)//��ò��ε�Ƶ��
{
    SetPowerWave();
    trans1 b;//����һ���ṹ�壬���ڴ�ŷ�ֵ��Ƶ��
    HAL_Delay(10);
    int max = ADC_Value[0];
    int min = ADC_Value[0];

    for (int i = 0; i < 200; i++) {
        if (ADC_Value[i] > max) {
            max = ADC_Value[i];
        }
    }

    for (int i = 0; i < 200; i++) {
        if (ADC_Value[i] < min) {
            min = ADC_Value[i];
        }
    }

    int minline = (max + min) / 2;
//        LOG_PRINTF("max is %d\n", max);
//        LOG_PRINTF("min is %d\n", min);
//        LOG_PRINTF("minline is %d\n", minline);

    int count = 0;
    int biaozhi = 0;
    if (ADC_Value[50] > minline) {
        for (int i = 50; i < 200; i++) {
            if (ADC_Value[i] < minline) {
                count++;
                biaozhi = 1;
            }
            if ((ADC_Value[i] > minline) && (biaozhi == 1)) {
                break;
            }
        }

    }

    if (ADC_Value[50] < minline) {
        for (int i = 50; i < 200; i++) {
            if (ADC_Value[i] > minline) {
                count++;
                biaozhi = 1;
            }
            if ((ADC_Value[i] < minline) && (biaozhi == 1)) {
                break;
            }
        }

    }

//  LOG_PRINTF("count is %d\n", count);
//  LOG_PRINTF("pinlv is %dhz\n", 860000 / (count * 2));
//  LOG_PRINTF("a0is %d\n", ADC_Value[0]);
    b.Amplitude = (max - min);
    b.Freq = 860000 / (count * 2);
    return b;

}
uint16_t *TransformAllWave(void)//16λ�����������ܱ�ʾ���������65536
{
    uint16_t i;
    for (i = 0; i < GETWAVENUMBER ; i ++) {
        ONEWAVE[i * 2] = ADC_Value[i] & 0XFF; //�õ���ѹֵ�ĵͰ�λ
        ONEWAVE[i * 2 + 1] = ADC_Value[i] >> 8; //�õ���ѹֵ�ĸ߰�λ
        Adj714AllZero();
        HAL_Delay(10);
    }
    return ONEWAVE;
}
