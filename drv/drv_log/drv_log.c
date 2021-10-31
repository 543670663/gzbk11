#include "./drv_log/drv_log.h"
#include "drv_log.h"
#include "usart.h"
#include "string.h"
#include "string.h"
void SendLog(void *str)
{
    uint16_t len = strlen(str);
    len = ((len > LOG_BUF_SIZE) ? LOG_BUF_SIZE : len);
    HAL_UART_Transmit(&huart1, (uint8_t *)str, len, HAL_MAX_DELAY);// 参数1：UATR的别名&huart1 Timeout；参数2：需要发送的数据； 参数3：发送的字节数：参数4：发送数据超过该时间退出发送
}
