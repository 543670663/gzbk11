#include "drv_ad9837.h"
#include "spi.h"
#include "./drv_ad9837/drv_ad9837.h"

uint16_t FRQLW = 0;    // MSB of Frequency Tuning Word
uint16_t FRQHW = 0;    // LSB of Frequency Tuning Word
uint32_t  phaseVal = 0; // Phase Tuning Value
uint8_t WKNOWN = 0;    // Flag Variable



void AD9837_SetRegisterValue(uint16_t regValue)
{
    unsigned char data[2] = {0x00, 0x00};
    data[0] = (unsigned char)((regValue & 0xFF00) >> 8);
    data[1] = (unsigned char)((regValue & 0x00FF) >> 0);
    AD9837_CS_0;
    HAL_SPI_Transmit(&hspi2, data, 2, 50);
    AD9837_CS_1;
}

void AD9837_Resert(void)
{
    AD9837_SetRegisterValue(0X2100);//D13=1,D8=1,reset
}

void AD9837_SetWave(uint16_t Wave)
{
    switch (Wave) {
        case 0:
            AD9837_SetRegisterValue(0x2000); // Value for 正弦 Wave
            break;
        case 1:
            AD9837_SetRegisterValue(0x2028); // Value for方波 Wave
            break;
        case 2:
            AD9837_SetRegisterValue(0x2002); // Value for 三角波 Wave

            break;
        default:
            break;
    }
}

// ------------------------------------------------ Sets Wave Frequency & Phase (In Degree) In PHASE0 & FREQ0 Registers
void AD9837_SetWaveData(float Frequency, float Phase, uint16_t Wave)
{
    HAL_Delay(1);

// ---------- Tuning Word for Phase ( 0 - 360 Degree )   相位设置
    if (Phase > 360){Phase = 360;} //相位的最大值（度数）
    phaseVal  = ((int32_t)(Phase * (4096 / 360))) | 0XC000; // 4096/360 = 11.37 寄存器每度变化并使用 0xC000，即第 0 阶段寄存器地址

// ---------- Tuning word for Frequency  频率设置  寄存器的值 = 频率*2^28/16m（2^28 = 268,435,456）
    long freq = 0;
    freq = (int32_t)(Frequency * 16.8 + 1); // Tuning Word
    FRQHW = (int32_t)((freq & 0xFFFC000) >> 14); // FREQ MSB
    FRQLW = (int32_t)(freq & 0x3FFF); // FREQ LSB
    FRQLW |= 0x4000;
    FRQHW |= 0x4000;
// ------------------------------------------------ Writing DATA
    HAL_Delay(1);
    AD9837_SetRegisterValue(0x2100); // enable 16bit words and set reset bit允许写入完整的频率
    AD9837_SetRegisterValue(FRQLW);
    AD9837_SetRegisterValue(FRQHW);
    AD9837_SetRegisterValue(phaseVal);
    AD9837_SetWave(Wave);
    return;
}
