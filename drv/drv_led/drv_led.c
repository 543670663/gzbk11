#include "./drv_led/drv_led.h"
#include "drv_led.h"
#include "gpio.h"

GpioDevStruct LED[] = {
    {GPIOB, GPIO_PIN_2, GPIO_PIN_SET}, 
};

/***********************************************************************
*@功能描述:               设置LED灯状态;
*@参数:    led：          小灯引脚
           state          小灯状态
 **********************************************************************/
void SetLedState(LedEnum led, LedStateEnum state)
{
    switch (state) {
        case LED_ON:
            HAL_GPIO_WritePin( LED[led].gpio, LED[led].gpioPin,  LED[led].workState);
            break;
        case LED_OFF:
            HAL_GPIO_WritePin( LED[led].gpio, LED[led].gpioPin, (GPIO_PinState)!LED[led].workState);
            break;
        case LED_TOGGLE:
            HAL_GPIO_TogglePin(LED[led].gpio, LED[led].gpioPin);
            break;
        default:
            ;
    }
}
