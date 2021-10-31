#include "drv_led.h"
#include "drv_log.h"
#include "drv_595.h"
#include <stdio.h>
#include "stdbool.h"
#include "usbd_cdc_if.h"
#include "drv_access_con.h"
#include "drv_power_gpio.h"
#include "drv_power_vol.h"
#include "comm_prot.h"
#include "comm_prot_def.h"
#include "drv_adg714.h"
#include "drv_ad9837.h"
#include "drv_power_wave.h"
#include "drv_power_dds.h"
#include "ring_buffer.h"
#include "drv_cmd.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "./drv_cmd/drv_cmd.h"

#define UART_RRB_SIZE 128                  //队列长度

extern uint16_t GetAllVolAuto;
static CommDevStruct dev;
uint8_t aRxBuffer1[10];                    //用来接收串口1发送的数据

static RingBufStruct RxRing;               //初始化队列数组
static unsigned char RxBuff[UART_RRB_SIZE];//定义队列

static uint32_t SendByte(uint8_t *bytes, uint32_t len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)bytes, len, HAL_MAX_DELAY);
    return len;
}

static void LedCmd(CommPackStruct *pack)
{
    SetLedState(LED_1, pack->data[0] ? LED_ON : LED_OFF);
}

static void RelayCmd(CommPackStruct *pack)
{
    HAL_Delay(1);
    RelaySetOut(pack->data[0], (RelayStateEnum)pack->data[1]);
}

static void CableCmd(CommPackStruct *pack)
{
    HAL_Delay(1);
    CableSet(pack->data[0], (RelayStateEnum)pack->data[1]);
}

static void RelayZeroCmd(CommPackStruct *pack)
{
    HAL_Delay(1);
    RelaySetZero();
    RelaySetOut(0, (RelayStateEnum)0);
}

static void GetOneVolCmd(CommPackStruct *pack)
{
    HAL_Delay(1);
    SetPowerVol((PowerVolEnum)pack->data[0]);
    HAL_Delay(1);
    LogPowerState((PowerVolEnum)pack->data[0]);
}

static void GetWaveCmd(CommPackStruct *pack)
{
    HAL_Delay(1);
    Adj714Write(2, pack->data[0] + 1);
    SetPowerWave();
    HAL_Delay(1);

    uint16_t i;
    uint16_t *WaveVolCmd;
    WaveVolCmd = TransformAllWave();
    uint8_t SendWaveVol[SENDWAVENUMBER];
    for (i = 0; i < SENDWAVENUMBER ; i ++) {
        SendWaveVol[i] = WaveVolCmd[i];
    }
    CommSendData(&dev, 0x01, CMD_GET_ONE_WAVE, SendWaveVol,  200, false);
}

static void GetWaveFreqCmd(CommPackStruct *pack)
{
    trans1 a;
    HAL_Delay(1);
    Adj714Write(2, pack->data[0] + 1);
    a = GetPowerWaveFreq();

    HAL_Delay(1);
//    a.Amplitude=2.33*a.Amplitude-313.1;
    LOG_PRINTF("Pinlv is %dhz\n",  a.Freq);
    LOG_PRINTF("Fuzhi is %dmv\n",  a.Amplitude);

    uint8_t Wave_Message[5];

    Wave_Message[0] = a.Freq & 0XFF;
    Wave_Message[1] = a.Freq >> 8;
    Wave_Message[2] = a.Freq >> 16;
    Wave_Message[3] = a.Amplitude & 0XFF;
    Wave_Message[4] = a.Amplitude >> 8;
    CommSendData(&dev, 0x01, CMD_GET_ONE_WAVE_FREQ, Wave_Message, 5, false);
}

static void DdsOutCmd(CommPackStruct *pack)
{
    AD9837_Init(pack->data[0], (pack->data[1]) * 100000, pack->data[2] * 10, (PowerDdsEnum)pack->data[3]);
}

static void DdsResertCmd(CommPackStruct *pack)
{
    AD9837_Resert();
}

static void GetGpioCmd(CommPackStruct *pack)
{
    uint8_t Gpio_State;
    Gpio_State = LogPowerWorkState();
    CommSendData(&dev, 0x01, CMD_GET_GPIO, &Gpio_State, 1, false);
}

static void GetVersionCmd()
{
    uint8_t Version[1] = {0x06};//版本号
    LOG_DEBUG("版本号为V_%d",  Version[0]);
    CommSendData(&dev, 0x01, CMD_LOG_VERSION, Version, 1, false);
}

void GetAllVolCmd(void)// 获得全部的电压值并且上传上位机
{
    uint8_t i;
    uint16_t *AllVolCmd;
    AllVolCmd = GetAllVol();
    uint8_t SendAllVol[SENDVOLNUMBER];
    for (i = 0; i < SENDVOLNUMBER ; i ++) {
        SendAllVol[i] = AllVolCmd[i];
    }
    CommSendData(&dev, 0x01, CMD_GET_ALL_VOL, SendAllVol,  sizeof(SendAllVol) / sizeof(SendAllVol[0]), false);
}

static void RecvPackCallback(CommPackStruct *pack)
{
    ProcessCmd(pack);
}

void CommInit(void)//通信初始化
{
    CommPrintInit((PrintFptr) SendLog);//传参为打印的函数
    CommStructInit(&dev, 0xAA, SendByte, RecvPackCallback);
    RingBufInit(&RxRing, RxBuff, sizeof(RxBuff[0]), UART_RRB_SIZE);//队列初始化
    HAL_UART_Receive_IT(&huart1, aRxBuffer1, 1);          //开启接收中断
}

void ProcessCmd(CommPackStruct *pack)
{
    switch (pack->cmd) {
        case CMD_LED1:
            LedCmd(pack);
            break;

        case CMD_RELAY :
            RelayCmd(pack);
            break;

        case CMD_CABLE :
            CableCmd(pack);
            break;

        case CMD_RELAY_ZERO :
            RelayZeroCmd(pack);
            break;

        case CMD_GET_ONE_VOL:
            GetOneVolCmd(pack);
            break;

        case CMD_GET_ONE_WAVE:
            GetWaveCmd(pack);
            break;

        case CMD_GET_ONE_WAVE_FREQ:
            GetWaveFreqCmd(pack);
            break;

        case CMD_DDS_OUT:
            DdsOutCmd(pack);
            break;

        case CMD_DDS_RESERT:
            DdsResertCmd(pack);
            break;

        case CMD_GET_GPIO:
            GetGpioCmd(pack);
            break;

        case CMD_GET_ALL_VOL:
            GetAllVolCmd();
            break;

        case CMD_LOG_VERSION  :
            GetVersionCmd();
            break;

        default:
            break;
    }
}

void QueueOut()//while循环里出队
{
    static uint8_t recv[UART_RRB_SIZE] = {0};
    uint16_t count = GetRingBufCount(&RxRing);      //队列里放的数
    if (count > 0) {                                //如果队列里面有数
        RingBufPopMult(&RxRing, recv, count);       //出队
        CommRecvData(&dev, recv, count);            //出队同时传输到通信协议
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//串口中断里入队
{
    HAL_UART_Receive_IT(&huart1, aRxBuffer1, 1);       //重新使能串口1接收中断
    RingBufInsert(&RxRing, aRxBuffer1);                //入队
}

