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
*@功能描述:               通过轮询的方式来获取电压值;
*@函数位置：              不需要，供LogPowerState(PowerVolEnum pwr_idx)调用
 **********************************************************************/
static void GetPowerState()//打开adc通道并且得到电压，存放在AD_Value1和AD_Value1   AD_Value1：低中高  AD_Value2：负电压
{
    HAL_ADCEx_Calibration_Start(&hadc1);//消除误差
    HAL_ADC_Start(&hadc1);//启动ADC装换PA0
    HAL_ADC_PollForConversion(&hadc1, 50); //等待转换完成,第二个参数表示超时时间,单位ms.
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
        AD_Value1 = HAL_ADC_GetValue(&hadc1);//读取ADC转换数据，数据为12位
    }

    HAL_ADCEx_Calibration_Start(&hadc2);//消除误差
    HAL_ADC_Start(&hadc2);//启动ADC装换PA1
    HAL_ADC_PollForConversion(&hadc2, 50); //等待转换完成,第二个参数表示超时时间,单位ms.
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC)) {
        AD_Value2 = HAL_ADC_GetValue(&hadc2);//读取ADC转换数据，数据为12位
    }

}

/***********************************************************************
*@功能描述:               打来对应的所采集电压714模拟开关;
*@参数:  pwr_idx：        需要采集的电压
*@函数位置：              主函数
**********************************************************************/
void SetPowerVol(PowerVolEnum pwr_idx)//电压采集波形采集  根据所打开开关 转换adc取的值
{
    Adj714Write(VOLADG714[pwr_idx].adg,  VOLADG714[pwr_idx].regno);//打开一极开关

    if((int8_t)pwr_idx >= 0 && pwr_idx <= 7) {    //打开二极开关
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
*@功能描述:               计算采集到的电压值，并且根据计算结果打印出来;
*@参数:  pwr_idx：        需要采集的电压
*@函数位置：              while(1)主循环
*@return ：               电压值
 **********************************************************************/
uint16_t LogPowerState(PowerVolEnum pwr_idx)//打开adc通道并且得到电压，存放在AD_Value1和AD_Value1   AD_Value1：低中高  AD_Value2：负电压
{
    GetPowerState();
    if((int8_t)pwr_idx >= 0 && pwr_idx <= 7) {
        vol = (AD_Value1 * 3.3 / 4096 ) * 2 * 1000;   
        LOG_PRINTF("[main]info :v（%s）= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx >= 8 && pwr_idx <= 23) {  
        vol = (AD_Value1 * 3.3 / 4096 ) * 6 * 1000;
        LOG_PRINTF("[main]info :v（%s）= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx >= 24 && pwr_idx <= 31) {
        vol = (AD_Value1 * 3.3 / 4096 ) * 71.5 * 1000;  
        LOG_PRINTF("[main]info :v（%s）= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx >= 32 && pwr_idx <= 35) {
        vol = (AD_Value2 * 3.3 / 4096 ) * 6 * 1000; 
        LOG_PRINTF("[main]info :v（%s）= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx >= 36 && pwr_idx <= 39) {
        vol = (AD_Value2 * 3.3 / 4096 ) * 71.5 * 1000;       
        LOG_PRINTF("[main]info :v（%s）= %dmv\r", VOLNAME[pwr_idx], vol);

        return vol;
    }
    if(pwr_idx == 40) {//关闭所有开关，用作测试证明adc采集是好的
        LOG_PRINTF("[main]info :v（%s）= %fmv\r\n v（%s）= %fmv\r\n\n", "AD_Value1", AD_Value1 * 3.3 / 4096, "AD_Value2", AD_Value2 * 3.3 / 4096);
        return 0;

    }
    return 1;
}

/***********************************************************************
*@功能描述:               获得全部电压值;
*@参数:  pwr_idx：        无
*@函数位置：              while(1)主循环
 **********************************************************************/
uint16_t* GetAllVol(void)//16位二进制数所能表示的最大数是65536
{
    uint16_t ALLVOL[GETVOLNUMBER];
    uint8_t i;
    for(i = 0; i < GETVOLNUMBER ; i ++) {
        HAL_Delay(10);
        SetPowerVol((PowerVolEnum)i);
        HAL_Delay(10);
        ALLVOL[i] = LogPowerState((PowerVolEnum)i);
        ALLVOLCMD[i * 2] = ALLVOL[i] & 0XFF; //得到电压值的低八位
        ALLVOLCMD[i * 2 + 1] = ALLVOL[i] >> 8; //得到电压值的高八位
        Adj714AllZero();
        HAL_Delay(10);
    }
    return ALLVOLCMD;
}
