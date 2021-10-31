#include "drv_adg714.h"
#include "spi.h"
#include "./drv_adg714/drv_adg714.h"


static uint8_t TEXT_Buffer[ADG714AMOUNT] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void Adj714Resert()//芯片复位
{
    HAL_GPIO_WritePin(SPI1_RESET_GPIO_Port, SPI1_RESET_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SPI1_RESET_GPIO_Port, SPI1_RESET_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
}

void Adj714Write(uint8_t adg, uint8_t regno) //adg：第adg个adg714芯片（1-8）    regno：芯片上的第regno个开关
{

    Adj714Resert();//芯片复位
    TEXT_Buffer[ADG714AMOUNT - adg] = TEXT_Buffer[ADG714AMOUNT - adg] | (0x01 << (regno - 1));
    ADG714_CS_0;//拉低片选信号cs
    HAL_SPI_Transmit(&hspi1, TEXT_Buffer, ADG714AMOUNT, 50);//发送写寄存器命令
    ADG714_CS_1;
}

void Adj714AllZero(void) //将714数据全部置零
{
    uint8_t j;
    for(j = 0; j <= ADG714AMOUNT ; j ++) {
        TEXT_Buffer[j] = 0;
    }
}
