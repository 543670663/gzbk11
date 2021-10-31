#include "./drv_access_con/drv_access_con.h"
#include "drv_access_con.h"
#include "drv_595.h"

static uint8_t Hc595_DATE[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //八组595控制的64个继电器，数组标号小的对应级联处于后面的继电器。例子：kc1由Hc595_DATE[7]的第1位决定

static void  CableSetZero(void)
{
    for (int i = 2; i < 4 ; i ++) {
        Hc595_DATE[i] = 0x00;
    }
}
/***********************************************************************
*@功能描述:               控制前后端工装板的所有继电器状态;
*@参数:  relay:           枚举64个继电器;
         state:           开或关;
*@使用方式:               直接用relay函数,relay取值为1-25，49-对应后端工装板24个继电器。
                          如果用CableSet函数，relay取值为25-64，对应前端工装板40个继电器
 **********************************************************************/

void  RelaySet(uint8_t relay, RelayStateEnum state)
{
    uint8_t which_hc595;//代表要打开的继电器是由第i+1块595芯片控制
    uint8_t which_bit;  //代表要置1的位是地i块的第j位

    which_hc595 = relay / 8.1 ;//8.1是因为如果relay是8要让i为0，8是在第（i+1=1）块595上
    which_bit = relay - 8 * which_hc595 - 1;

    uint8_t tmp = 0x01 << which_bit;

    switch (state / state) {
        case 1:
            Hc595_DATE[7 - which_hc595] = Hc595_DATE[7 - which_hc595] | tmp;//对数组中继电器对应的值进行改变
            break;

        case 0:
            tmp = ~tmp;
            Hc595_DATE[7 - which_hc595] = Hc595_DATE[7 - which_hc595] & tmp;
            break;

        default:
            ;
    }
}

void  RelaySetOut(uint8_t relay, RelayStateEnum state)
{
    RelaySet(relay,  state);
    Hc595TransmitMultiByte(Hc595_DATE, 8);
}
/***********************************************************************
*@功能描述:               设置前端工装板通道的状态;
*@参数:  cable:           通道对应数字 如CABLE10 则为10+24=34;
         state:           CABLE_SHIBOQI或CABLE_XINHAO，表示分别将通道接到示波器和信号发生器;
 **********************************************************************/
void  CableSet(uint8_t cable, RelayStateEnum state)
{
    CableSetZero();
   
    assert_param(cable < 128);
    uint8_t first_relay = ((cable % 16 / 2) + 17 + 24); //要打开的一级继电器
    uint8_t second_relay = ((cable / 16) + 9 + 24) ; //二级继电器
    uint8_t third_relay = ((cable / 32) + 5 + 24); //三级继电器

    if (cable % 2 == 0) { //四级继电器
        RelaySet(27, state);
    } else {
        RelaySet(28, state);
    }

    RelaySet(first_relay, state);
    RelaySet(second_relay, state);
    RelaySet(third_relay, state);

    if (state == CABLE_SHIBOQI) { //四级继电器
        RelaySet(25, state);
    }
    if (state == CABLE_XINHAO) { //四级继电器
        RelaySet(26, state);
    }
    Hc595TransmitMultiByte(Hc595_DATE, 8);
}

void  RelaySetZero(void)
{
    for (int i = 0; i < 8 ; i ++) {
        Hc595_DATE[i] = 0x00;
    }
}

