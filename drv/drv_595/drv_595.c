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
*@功能描述:               将一个二进制字节一位一位移入移位寄存器中;
*@参数:    byte           将要移入移位寄存器的二进制数
 **********************************************************************/
static void Hc595PutInByte(uint8_t byte)
{
    uint8_t i;
    for(i = 0; i < 8; i ++) {  //一个字节8位，传输8次，一次一位，循环8次，刚好移完8位
        /****  步骤1：将数据传到DS引脚    ****/
        if(byte & 0x80) {        //先传输高位，通过与运算判断第八是否为1
            SetHc595State(HC595_Data_DS, High);    //如果第八位是1，则与 595 DS连接的引脚输出高电平
            LOG_PRINTF("1");
        } else {                 //否则输出低电平
            SetHc595State(HC595_Data_DS, Low);
            LOG_PRINTF("0");
        }

        /*** 步骤2：SHCP每产生一个上升沿，当前的bit就被送入移位寄存器 ***/
        SetHc595State(HC595_SHCP, Low);   // SHCP拉低
        HAL_Delay(1);
        SetHc595State(HC595_SHCP, High);  // SHCP拉高， SHCP产生上升沿 ，弹夹下一位
        HAL_Delay(1);
        byte <<= 1;     // 左移一位，将低位往高位移，通过   if (byte & 0x80)判断低位是否为1
    }

}

/***********************************************************************
*@功能描述:               移位寄存器的数据移入存储寄存器
 **********************************************************************/
static void Hc595TransmitByte(void)
{
    /**  步骤3：STCP产生一个上升沿，移位寄存器的数据移入存储寄存器  **/
    SetHc595State(HC595_STCP, Low);
    HAL_Delay(1);
    SetHc595State(HC595_STCP, High);
    HAL_Delay(1);
}

/***********************************************************************
*@功能描述:               便于给级联了多片的595发送数据;
*@参数:    *data：        要发送的存放数据的数组的首地址值，数据为8位2进制数。
           len            传授数组中数据的个数
 **********************************************************************/
void Hc595TransmitMultiByte(uint8_t *data, uint8_t len)
{
    SetHc595State(HC595_OE, Low);
    SetHc595State(HC595_MR, High);

    for(uint8_t i = 0; i < len; i ++) {  // 共发送len个字节
        LOG_PRINTF("--------%d", (HC595AMOUNT - i)* HC595AMOUNT);
        Hc595PutInByte(data[i]);
    }
    Hc595TransmitByte(); //弹夹装满，使能输出
    LOG_PRINTF("---------------");
}
