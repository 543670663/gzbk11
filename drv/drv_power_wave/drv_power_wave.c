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

void SetPowerWave()//根据所打开开关 转换adc取的值
{
    HAL_ADCEx_Calibration_Start(&hadc3);//消除误差
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)ADC_Value, sizeof(ADC_Value) / sizeof(ADC_Value[0])); //打开dma波形采集
    while ((DMA2->ISR & 0x20000) == 0);
    HAL_ADC_Stop_DMA(&hadc3);//关闭dma波形采集

    for (uint32_t i = 0; i < sizeof(ADC_Value) / sizeof(ADC_Value[0]); i++) { //将缓存的电压值打印
        ADC_Value[i] =  ADC_Value[i] * 3.3 / 4096 * 1000;
        LOG_PRINTF("%02dmv", ADC_Value[i]);
    }
}

trans1 GetPowerWaveFreq(void)//获得波形的频率
{
    SetPowerWave();
    trans1 b;//定义一个结构体，用于存放幅值和频率
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
uint16_t *TransformAllWave(void)//16位二进制数所能表示的最大数是65536
{
    uint16_t i;
    for (i = 0; i < GETWAVENUMBER ; i ++) {
        ONEWAVE[i * 2] = ADC_Value[i] & 0XFF; //得到电压值的低八位
        ONEWAVE[i * 2 + 1] = ADC_Value[i] >> 8; //得到电压值的高八位
        Adj714AllZero();
        HAL_Delay(10);
    }
    return ONEWAVE;
}
