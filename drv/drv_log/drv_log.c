#include "./drv_log/drv_log.h"
#include "drv_log.h"
#include "usart.h"
#include "string.h"
#include "string.h"
void SendLog(void *str)
{
    uint16_t len = strlen(str);
    len = ((len > LOG_BUF_SIZE) ? LOG_BUF_SIZE : len);
    HAL_UART_Transmit(&huart1, (uint8_t *)str, len, HAL_MAX_DELAY);// ����1��UATR�ı���&huart1 Timeout������2����Ҫ���͵����ݣ� ����3�����͵��ֽ���������4���������ݳ�����ʱ���˳�����
}
