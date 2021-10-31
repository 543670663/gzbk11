#include "./drv_access_con/drv_access_con.h"
#include "drv_access_con.h"
#include "drv_595.h"

static uint8_t Hc595_DATE[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //����595���Ƶ�64���̵�����������С�Ķ�Ӧ�������ں���ļ̵��������ӣ�kc1��Hc595_DATE[7]�ĵ�1λ����

static void  CableSetZero(void)
{
    for (int i = 2; i < 4 ; i ++) {
        Hc595_DATE[i] = 0x00;
    }
}
/***********************************************************************
*@��������:               ����ǰ��˹�װ������м̵���״̬;
*@����:  relay:           ö��64���̵���;
         state:           �����;
*@ʹ�÷�ʽ:               ֱ����relay����,relayȡֵΪ1-25��49-��Ӧ��˹�װ��24���̵�����
                          �����CableSet������relayȡֵΪ25-64����Ӧǰ�˹�װ��40���̵���
 **********************************************************************/

void  RelaySet(uint8_t relay, RelayStateEnum state)
{
    uint8_t which_hc595;//����Ҫ�򿪵ļ̵������ɵ�i+1��595оƬ����
    uint8_t which_bit;  //����Ҫ��1��λ�ǵ�i��ĵ�jλ

    which_hc595 = relay / 8.1 ;//8.1����Ϊ���relay��8Ҫ��iΪ0��8���ڵڣ�i+1=1����595��
    which_bit = relay - 8 * which_hc595 - 1;

    uint8_t tmp = 0x01 << which_bit;

    switch (state / state) {
        case 1:
            Hc595_DATE[7 - which_hc595] = Hc595_DATE[7 - which_hc595] | tmp;//�������м̵�����Ӧ��ֵ���иı�
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
*@��������:               ����ǰ�˹�װ��ͨ����״̬;
*@����:  cable:           ͨ����Ӧ���� ��CABLE10 ��Ϊ10+24=34;
         state:           CABLE_SHIBOQI��CABLE_XINHAO����ʾ�ֱ�ͨ���ӵ�ʾ�������źŷ�����;
 **********************************************************************/
void  CableSet(uint8_t cable, RelayStateEnum state)
{
    CableSetZero();
   
    assert_param(cable < 128);
    uint8_t first_relay = ((cable % 16 / 2) + 17 + 24); //Ҫ�򿪵�һ���̵���
    uint8_t second_relay = ((cable / 16) + 9 + 24) ; //�����̵���
    uint8_t third_relay = ((cable / 32) + 5 + 24); //�����̵���

    if (cable % 2 == 0) { //�ļ��̵���
        RelaySet(27, state);
    } else {
        RelaySet(28, state);
    }

    RelaySet(first_relay, state);
    RelaySet(second_relay, state);
    RelaySet(third_relay, state);

    if (state == CABLE_SHIBOQI) { //�ļ��̵���
        RelaySet(25, state);
    }
    if (state == CABLE_XINHAO) { //�ļ��̵���
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

