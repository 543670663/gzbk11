#include "./drv_595/drv_595.h"
#include "drv_595.h"
#include "gpio.h"
#include "drv_log.h"
#include <stdio.h>


GpioDevStruct HC595[] = {
    {GPIOC, GPIO_PIN_8,  GPIO_PIN_SET},
    {GPIOC, GPIO_PIN_9,  GPIO_PIN_SET},
    {GPIOC, GPIO_PIN_10, GPIO_PIN_SET},
    {GPIOC, GPIO_PIN_11, GPIO_PIN_SET},
    {GPIOC, GPIO_PIN_12, GPIO_PIN_SET},
};



static void SetHc595State(HC595_PinEnum connector, HC595_StateEnum state)
{
    switch(state) {
    case High:
        HAL_GPIO_WritePin(HC595[connector].gpio, HC595[connector].gpioPin,  HC595[connector].workState);
        break;
    case Low:
        HAL_GPIO_WritePin(HC595[connector].gpio, HC595[connector].gpioPin, (GPIO_PinState)!HC595[connector].workState);
        break;
    default:
        ;
        //default: WS_ERROR();
    }
}


/***********************************************************************
*@��������:               ��һ���������ֽ�һλһλ������λ�Ĵ�����;
*@����:    byte           ��Ҫ������λ�Ĵ����Ķ�������
 **********************************************************************/
static void Hc595PutInByte(uint8_t byte)
{
    uint8_t i;
    for(i = 0; i < 8; i ++) {  //һ���ֽ�8λ������8�Σ�һ��һλ��ѭ��8�Σ��պ�����8λ
        /****  ����1�������ݴ���DS����    ****/
        if(byte & 0x80) {        //�ȴ����λ��ͨ���������жϵڰ��Ƿ�Ϊ1
            SetHc595State(HC595_Data_DS, High);    //����ڰ�λ��1������ 595 DS���ӵ���������ߵ�ƽ
            LOG_PRINTF("1");
        } else {                 //��������͵�ƽ
            SetHc595State(HC595_Data_DS, Low);
            LOG_PRINTF("0");
        }

        /*** ����2��SHCPÿ����һ�������أ���ǰ��bit�ͱ�������λ�Ĵ��� ***/
        SetHc595State(HC595_SHCP, Low);   // SHCP����
        HAL_Delay(1);
        SetHc595State(HC595_SHCP, High);  // SHCP���ߣ� SHCP���������� ��������һλ
        HAL_Delay(1);
        byte <<= 1;     // ����һλ������λ����λ�ƣ�ͨ��   if (byte & 0x80)�жϵ�λ�Ƿ�Ϊ1
    }

}

/***********************************************************************
*@��������:               ��λ�Ĵ�������������洢�Ĵ���
 **********************************************************************/
static void Hc595TransmitByte(void)
{
    /**  ����3��STCP����һ�������أ���λ�Ĵ�������������洢�Ĵ���  **/
    SetHc595State(HC595_STCP, Low);
    HAL_Delay(1);
    SetHc595State(HC595_STCP, High);
    HAL_Delay(1);
}

/***********************************************************************
*@��������:               ���ڸ������˶�Ƭ��595��������;
*@����:    *data��        Ҫ���͵Ĵ�����ݵ�������׵�ֵַ������Ϊ8λ2��������
           len            �������������ݵĸ���
 **********************************************************************/
void Hc595TransmitMultiByte(uint8_t *data, uint8_t len)
{
    SetHc595State(HC595_OE, Low);
    SetHc595State(HC595_MR, High);

    for(uint8_t i = 0; i < len; i ++) {  // ������len���ֽ�
        LOG_PRINTF("--------%d", (HC595AMOUNT - i)* HC595AMOUNT);
        Hc595PutInByte(data[i]);
    }
    Hc595TransmitByte(); //����װ����ʹ�����
    LOG_PRINTF("---------------");
}
