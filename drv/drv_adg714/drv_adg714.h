
#ifndef __DRV_ADG714_H
#define __DRV_ADG714_H

#include "stm32f1xx.h"

#define ADG714_CS_0  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
#define ADG714_CS_1  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
#define ADG714AMOUNT 8  /* ADG714菊花链的连接的片数*/

typedef struct {
    uint8_t adg;
    uint8_t regno;
} Ad714Struct;

void Adj714Write(uint8_t adg,uint8_t regno);
void Adj714Resert(void);
void Adj714AllZero(void);
#endif
