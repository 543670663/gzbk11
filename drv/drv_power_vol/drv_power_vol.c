#include "./drv_power_vol/drv_power_vol.h"
#include "drv_power_vol.h"
#include "drv_log.h"
#include "adc.h"
#include "drv_adg714.h"
#include <stdio.h>
#include <stdlib.h>

Ad714Struct VOLADG714[] = {
    {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8},
    {4, 1}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, {4, 6}, {4, 7}, {4, 8},
    {5, 1}, {5, 2}, {5, 3}, {5, 4}, {5, 5}, {5, 6}, {5, 7}, {5, 8},
    {6, 1}, {6, 2}, {6, 3}, {6, 4}, {6, 5}, {6, 6}, {6, 7}, {6, 8},
    {7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {7, 7}, {7, 8},
    {0, 0}
};

char *VOLNAME[] = {
    "LOW_VOL1 ", "LOW_VOL2",  "LOW_VOL3",  "LOW_VOL4",  "LOW_VOL5",
    "LOW_VOL6",  " LOW_VOL7", "LOW_VOL8",  " MID_VOL1",  "MID_VOL2",
    "MID_VOL3",  "MID_VOL4",  "MID_VOL5",  "MID_VOL6",   "MID_VOL7",
    "MID_VOL8",  "MID_VOL9",  "MID_VOL10", "MID_VOL11",  "MID_VOL12",
    "MID_VOL13", "MID_VOL14", "MID_VOL15", " MID_VOL16", "HI_VOL1",
    "HI_VOL2",   "HI_VOL3",   "HI_VOL4",   "HI_VOL5",    "HI_VOL6",
    "HI_VOL7",   "HI_VOL8",
    "NEGVOL_TEST1", "NEGVOL_TEST2", "NEGVOL_TEST3", "NEGVOL_TEST4",
    "NEGVOL_TEST5", "NEGVOL_TEST6", "NEGVOL_TEST7", "NEGVOL_TEST8",
    "VOL_OFF"
};

uint16_t vol ;
uint16_t AD_Value1 = 0;
uint16_t AD_Value2 = 0;
uint16_t ALLVOLCMD[SENDVOLNUMBER];
/***********************************************************************
*@��������:               ͨ����ѯ�ķ�ʽ����ȡ��ѹֵ;
*@����λ�ã�              ����Ҫ����LogPowerState(PowerVolEnum pwr_idx)����
 **********************************************************************/
static void GetPowerState()//��adcͨ�����ҵõ���ѹ�������AD_Value1��AD_Value1   AD_Value1�����и�  AD_Value2������ѹ
{
    HAL_ADCEx_Calibration_Start(&hadc1);//�������
    HAL_ADC_Start(&hadc1);//����ADCװ��PA0
    HAL_ADC_PollForConversion(&hadc1, 50); //�ȴ�ת�����,�ڶ���������ʾ��ʱʱ��,��λms.
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
        AD_Value1 = HAL_ADC_GetValue(&hadc1);//��ȡADCת�����ݣ�����Ϊ12λ
    }

    HAL_ADCEx_Calibration_Start(&hadc2);//�������
    HAL_ADC_Start(&hadc2);//����ADCװ��PA1
    HAL_ADC_PollForConversion(&hadc2, 50); //�ȴ�ת�����,�ڶ���������ʾ��ʱʱ��,��λms.
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC)) {
        AD_Value2 = HAL_ADC_GetValue(&hadc2);//��ȡADCת�����ݣ�����Ϊ12λ
    }

}

/***********************************************************************
*@��������:               ������Ӧ�����ɼ���ѹ714ģ�⿪��;
*@����:  pwr_idx��        ��Ҫ�ɼ��ĵ�ѹ
*@����λ�ã�              ������
**********************************************************************/
void SetPowerVol(PowerVolEnum pwr_idx)//��ѹ�ɼ����βɼ�  �������򿪿��� ת��adcȡ��ֵ
{
    Adj714Write(VOLADG714[pwr_idx].adg,  VOLADG714[pwr_idx].regno);//��һ������

    if((int8_t)pwr_idx >= 0 && pwr_idx <= 7) {    //�򿪶�������
        Adj714Write(8, 1);
    }
    if(pwr_idx >= 8 && pwr_idx <= 15) {
        Adj714Write(8, 2);
    }
    if(pwr_idx >= 16 && pwr_idx <= 23) {
        Adj714Write(8, 3);
    }
    if(pwr_idx >= 24 && pwr_idx <= 31) {
        Adj714Write(8, 4);
    }
}

/***********************************************************************
*@��������:               ����ɼ����ĵ�ѹֵ�����Ҹ��ݼ�������ӡ����;
*@����:  pwr_idx��        ��Ҫ�ɼ��ĵ�ѹ
*@����λ�ã�              while(1)��ѭ��
*@return ��               ��ѹֵ
 **********************************************************************/
uint16_t LogPowerState(PowerVolEnum pwr_idx)//��adcͨ�����ҵõ���ѹ�������AD_Value1��AD_Value1   AD_Value1�����и�  AD_Value2������ѹ
{
    GetPowerState();
    if((int8_t)pwr_idx >= 0 && pwr_idx <= 7) {
        vol = (AD_Value1 * 3.3 / 4096 ) * 2 * 1000;   
        LOG_PRINTF("[main]info :v��%s��= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx >= 8 && pwr_idx <= 23) {  
        vol = (AD_Value1 * 3.3 / 4096 ) * 6 * 1000;
        LOG_PRINTF("[main]info :v��%s��= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx >= 24 && pwr_idx <= 31) {
        vol = (AD_Value1 * 3.3 / 4096 ) * 71.5 * 1000;  
        LOG_PRINTF("[main]info :v��%s��= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx >= 32 && pwr_idx <= 35) {
        vol = (AD_Value2 * 3.3 / 4096 ) * 6 * 1000; 
        LOG_PRINTF("[main]info :v��%s��= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx >= 36 && pwr_idx <= 39) {
        vol = (AD_Value2 * 3.3 / 4096 ) * 71.5 * 1000;       
        LOG_PRINTF("[main]info :v��%s��= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx == 40) {//�ر����п��أ���������֤��adc�ɼ��Ǻõ�
        LOG_PRINTF("[main]info :v��%s��= %fmv\r\n v��%s��= %fmv\r\n\n", "AD_Value1", AD_Value1 * 3.3 / 4096, "AD_Value2", AD_Value2 * 3.3 / 4096);
        return 0;

    }
    return 1;
}

/***********************************************************************
*@��������:               ���ȫ����ѹֵ;
*@����:  pwr_idx��        ��
*@����λ�ã�              while(1)��ѭ��
 **********************************************************************/
uint16_t* GetAllVol(void)//16λ�����������ܱ�ʾ���������65536
{
    uint16_t ALLVOL[GETVOLNUMBER];
    uint8_t i;
    for(i = 0; i < GETVOLNUMBER ; i ++) {
        HAL_Delay(10);
        SetPowerVol((PowerVolEnum)i);
        HAL_Delay(10);
        ALLVOL[i] = LogPowerState((PowerVolEnum)i);
        ALLVOLCMD[i * 2] = ALLVOL[i] & 0XFF; //�õ���ѹֵ�ĵͰ�λ
        ALLVOLCMD[i * 2 + 1] = ALLVOL[i] >> 8; //�õ���ѹֵ�ĸ߰�λ
        Adj714AllZero();
        HAL_Delay(10);
    }
    return ALLVOLCMD;
}
