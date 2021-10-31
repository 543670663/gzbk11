#include "./drv_power_gpio/drv_power_gpio.h"
#include "drv_power_gpio.h"
#include "gpio.h"
#include "drv_log.h"
#include <stdio.h>

GpioDevStruct POWER[] = {
    {GPIOB, GPIO_PIN_8,  GPIO_PIN_SET},
    {GPIOB, GPIO_PIN_9,  GPIO_PIN_SET},
    {GPIOB, GPIO_PIN_10, GPIO_PIN_SET},
    {GPIOB, GPIO_PIN_11, GPIO_PIN_SET},
    {GPIOC, GPIO_PIN_1,  GPIO_PIN_SET},
    {GPIOC, GPIO_PIN_2,  GPIO_PIN_SET},
    {GPIOC, GPIO_PIN_3,  GPIO_PIN_SET},
    {GPIOC, GPIO_PIN_4,  GPIO_PIN_SET},
};

char *GPIOSTATUS[] = {
    "PB8_1_8V_OK",   "PB8_1_8V_NO",   "PB9_1_8V_OK",  "PB9_1_8V_NO",  "PB10_3_3V_OK",
    "PB10_3_3V_NO",  "PB11_3_3V_OK",  "PB11_3_3V_NO", "PC1_3_3V_OK",  "PC1_3_3V_NO",
    "PC2_3_3V_OK",   "PC2_3_3V_NO",   "PC3_5V_OK",    "PC3_5V_NO",    "PC4_5V_OK\r\n",
    "PC4_5V_NO\r\n"
};

/***********************************************************************
*@功能描述:               检查相应的引脚状态是否为工作状态;
*@返回值：                工作状态返回ture;
 **********************************************************************/
static bool IsPowerWorkOk(PowerGpioEnum power)
{
    return (HAL_GPIO_ReadPin(POWER[power].gpio, POWER[power].gpioPin) == POWER[power].workState) ?
           true : false;
}

/***********************************************************************
*@功能描述:               得到8个引脚电平的状态;
*@返回值：                一个8位二进制数的地址，二进制的每一位表示了一个GPIO的状态;
 **********************************************************************/
uint8_t LogPowerWorkState()
{
    uint8_t gpio_state ;
    for (uint8_t i = 0; i < 8; i ++) {
        if (IsPowerWorkOk((PowerGpioEnum)i)) {
            gpio_state = gpio_state | (0x01 << (7 - i));
            LOG_PRINTF("%s", GPIOSTATUS[2 * i]);
        } else {
            LOG_PRINTF("%s", GPIOSTATUS[2 * i + 1]);
            HAL_Delay(1);
        }
    }
}
