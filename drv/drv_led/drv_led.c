#include "./drv_led/drv_led.h"
#include "drv_led.h"
#include "gpio.h"

GpioDevStruct LED[] = {
    {GPIOB, GPIO_PIN_2, GPIO_PIN_SET}, 
};

/***********************************************************************
*@��������:               ����LED��״̬;
*@����:    led��          С������
           state          С��״̬
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
