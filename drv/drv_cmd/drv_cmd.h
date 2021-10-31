#ifndef __DRV_CMD_H
#define __DRV_CMD_H
#include "stm32f1xx.h"
#include "main.h"
#include "comm_prot.h"
#include "comm_prot_def.h"


/*-----------------------------------------------------------------------------
   交互式命令
------------------------------------------------------------------------------*/
#define CMD_LED1                        0x01A1

#define CMD_RELAY_ZERO                  0x00A2
#define CMD_RELAY                       0x01A2
#define CMD_CABLE                       0x02A2


#define CMD_GET_ONE_VOL                 0x01A3
#define CMD_GET_ONE_WAVE                0x02A3
#define CMD_GET_ONE_WAVE_FREQ           0x22A3
#define CMD_DDS_OUT                     0x03A3
#define CMD_DDS_RESERT                  0x05A3
#define CMD_GET_ALL_VOL                 0x04A3

#define CMD_GET_GPIO                    0x01A4

#define CMD_LOG_VERSION                 0x01A5//打印版本信息

uint32_t SendByte(uint8_t* bytes, uint32_t len);
extern void ProcessCmd(CommPackStruct* pack);
void CommInit(void);
void QueueOut(void);
void GetAllVolCmd(void);
void GetVersionCmd(void);
#endif
