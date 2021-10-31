#include "./drv_power_dds/drv_power_dds.h"
#include "drv_power_dds.h"
#include "drv_log.h"
#include "adc.h"
#include "drv_adg714.h"
#include <stdio.h>
#include "drv_ad9837.h"

Ad714Struct DDSADG714[] = {
    {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8},
};

char DDSNAME[DDSAMOUNT][DDSNAMELEN] = {
    "DDS_OUT1", "DDS_OUT2",  "DDS_OUT3",  "DDS_OUT4",  "DDS_OUT5",
    "DDS_OUT6", "DDS_OUT7",  "DDS_OUT8"
};

static void SetPowerDds(PowerDdsEnum pwr_idx)
{
    Adj714Write(DDSADG714[pwr_idx].adg,  DDSADG714[pwr_idx].regno);
}

void AD9837_Init(uint16_t WaveType, float FRQ, float Phase, PowerDdsEnum pwr_idx)
{
    HAL_Delay(1000);
    AD9837_SetWaveData(FRQ, Phase, WaveType);                        // Frequency & Phase Set
    SetPowerDds(pwr_idx);
    LOG_PRINTF("[main]info :dds(%s)=%uhz", DDSNAME[pwr_idx], (uint32_t)FRQ);
}


