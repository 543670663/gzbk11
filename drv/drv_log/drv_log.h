#ifndef __DRV_LOG_H_
#define __DRV_LOG_H_

#include "stm32f1xx.h"

void SendLog(void *str);

#ifdef  DEBUG

#define LOG_BUF_SIZE    256

#define LOG_PRINTF(format, ...) do                                          \
                                {                                           \
                                    char str[LOG_BUF_SIZE] = {0};           \
                                    sprintf(str, format, ##__VA_ARGS__);    \
                                    SendLog(str);                           \
                                    SendLog("\r\n");                        \
                                } while (0)
                                
#define LOG_NL(format, ...)     do                                          \
                                {                                           \
                                    char str[LOG_BUF_SIZE] = {0};           \
                                    sprintf(str, format, ##__VA_ARGS__);    \
                                    SendLog(str);                           \
                                } while (0)
                                
#define LOG_INFO(format, ...)   do                                          \
                                {                                           \
                                    char str[LOG_BUF_SIZE] = {0};           \
                                    sprintf(str, format, ##__VA_ARGS__);    \
                                    SendLog("¡¾log¡¿info:");                \
                                    SendLog(str);                           \
                                    SendLog("\r\n");                        \
                                } while (0)
#define LOG_DEBUG(format, ...)  do                                          \
                                {                                           \
                                    char str[LOG_BUF_SIZE] = {0};           \
                                    sprintf(str, format, ##__VA_ARGS__);    \
                                    SendLog("¡¾log¡¿debug:");               \
                                    SendLog(str);                           \
                                    SendLog("\r\n");                        \
                                } while (0)
#define LOG_ERROR(format, ...)  do                                          \
                                {                                           \
                                    char str[LOG_BUF_SIZE] = {0};           \
                                    sprintf(str, format, ##__VA_ARGS__);    \
                                    SendLog("¡¾log¡¿error:");               \
                                    SendLog(str);                           \
                                    SendLog("\r\n");                        \
                                } while (0)


#elif   defined(RELEASE)
#define LOG_BUF_SIZE    128
#define LOG_PRINTF(format, ...)
#define LOG_INFO(format, ...)
#define LOG_DEBUG(format, ...)
#define LOG_ERROR(format, ...)
#define LOG_NL(format, ...)
#endif


#endif

