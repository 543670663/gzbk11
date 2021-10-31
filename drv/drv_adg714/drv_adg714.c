#include "drv_adg714.h"
#include "spi.h"
#include "./drv_adg714/drv_adg714.h"


static uint8_t TEXT_Buffer[ADG714AMOUNT] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void Adj714Resert()//оƬ��λ
{
    HAL_GPIO_WritePin(SPI1_RESET_GPIO_Port, SPI1_RESET_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SPI1_RESET_GPIO_Port, SPI1_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

void Adj714Write(uint8_t adg, uint8_t regno) //adg����adg��adg714оƬ��1-8��    regno��оƬ�ϵĵ�regno������
{

    Adj714Resert();//оƬ��λ
    TEXT_Buffer[ADG714AMOUNT - adg] = TEXT_Buffer[ADG714AMOUNT - adg] | (0x01 << (regno - 1));
    ADG714_CS_0;//����Ƭѡ�ź�cs
    HAL_SPI_Transmit(&hspi1, TEXT_Buffer, ADG714AMOUNT, 50);//����д�Ĵ�������
    ADG714_CS_1;
}

void Adj714AllZero(void) //��714����ȫ������
{
    uint8_t j;
    for(j = 0; j <= ADG714AMOUNT ; j ++) {
        TEXT_Buffer[j] = 0;
    }
}
