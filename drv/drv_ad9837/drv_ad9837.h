
#ifndef __DRV_AD9837_H
#define __DRV_AD9837_H

#include "stm32f1xx.h"

#include <math.h>

#define FMCLK 16000000        // Master Clock On AD9837

#define AD9837_REG_CMD      (0 << 14)
#define AD9837_RESET        (1 << 8)

#define AD9837_CS_0        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET); // Add here your code.
#define AD9837_CS_1        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET); // Add here your code.

void AD9837_SetWaveData(float Frequency, float Phase, uint16_t Wave);   // Sets Wave Frequency & Phase
void AD9837_SetWave(uint16_t Wave);
void AD9837_SetRegisterValue(unsigned short regValue);
void AD9837_Resert(void);
#endif
