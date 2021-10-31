#ifndef __COMM_PROT_H
#define __COMM_PROT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COMM_PROT_UNIT_TEST 0

#if defined(_DEBUG) || defined(DEBUG)
#define LOG_BUF_SIZE        256    // 最大只能256
    #if defined(_MSC_VER)
    #define COMM_LOG(fmt, ...)  do {                                    \
                                    char str[LOG_BUF_SIZE] = { 0 };     \
                                    sprintf_s(str, LOG_BUF_SIZE, fmt, ##__VA_ARGS__);    \
                                    str[LOG_BUF_SIZE - 1] = 0;          \
                                    PrintFunc("\033[36m");              \
                                    PrintFunc(str);                     \
                                    PrintFunc("\033[0m");               \
                                } while (0);
    #elif defined(__GNUC__)
    #define  COMM_LOG(fmt, args...)     do {                                    \
                                            char str[LOG_BUF_SIZE] = { 0 };     \
                                            sprintf(str, fmt, ##args);          \
                                            str[LOG_BUF_SIZE - 1] = 0;          \
                                            PrintFunc(str);                     \
                                        } while (0)
    #else
    #define COMM_LOG(fmt, ...)  do                                          \
                                {                                           \
                                    char str[LOG_BUF_SIZE] = {0};           \
                                    sprintf(str, fmt, ##__VA_ARGS__);       \
                                    PrintFunc(str);                         \
                                    PrintFunc("\r\n");                      \
                                } while (0)               
    #endif
#else
#define COMM_LOG(fm, ...)
#endif

#define COMM_PROT_VERSION   0x000001 // 通信协议版本：Ver：第5、6字节.第2、3字节.第0、1字节 ， 即v0.01

#define PACK_CRC_BYTE_NUM   2        // CRC校验位字节数
#define PACK_MIN_LEN        9        // 最小包长
#define PACK_DATA_MAX_LEN   255      // 该值不能超过255
#define PACK_MAX_LEN        (PACK_MIN_LEN + PACK_DATA_MAX_LEN)


#define MAX_DATA_BUF_LEN    (PACK_MAX_LEN * 2) // 假设所有数据都转义，那所需空间为 最大包长*2


    typedef struct {
        bool needAck : 1;
        bool skipCrc16 : 1;
        bool : 1; 
        bool : 1;
        bool : 1;
        bool : 1;
        bool : 1;
        bool : 1;
    } FlagStruct;

    typedef struct {
        uint8_t sender;
        uint8_t receiver;
        uint16_t cmd;
        FlagStruct flag;
        uint8_t timestamp;
        uint8_t dataLen;
        uint8_t data[MAX_DATA_BUF_LEN - PACK_MIN_LEN]; 
                            /* data结尾内存可能对不齐，但不影响工作 */
        uint16_t crc16;     /* 因为data实际不定长，crc16需手动赋值  */
    } CommPackStruct;

    typedef void (*PrintFptr)(const char* const str);
    typedef uint32_t (*SendBytesFptr)(uint8_t* bytes, uint32_t len);
    typedef void (*RecvPackCallbackFptr)(CommPackStruct* pack);

    typedef struct {
        uint8_t currentDevice;
        bool gotHead;           // 判断是否已获取包头
        uint8_t timestamp;      // 时间戳
        uint16_t bufLen;                // 已缓存数据的长度
        uint8_t bufData[MAX_DATA_BUF_LEN]; // 缓存数据
        SendBytesFptr sendByteFunc;
        RecvPackCallbackFptr recvPackCallback;
    } CommDevStruct;


    void CommPrintInit(PrintFptr print_func);

    void CommStructInit(CommDevStruct* dev, uint8_t cur_dev,
                        SendBytesFptr send_bytes_func,
                        RecvPackCallbackFptr recv_pack_callback);

    void CommRecvData(CommDevStruct *dev, const uint8_t *const input, uint32_t input_len);

    bool CommSendData(CommDevStruct *dev, uint8_t target, uint16_t cmd, const uint8_t *const data, uint8_t len, bool need_ack);

    
#if COMM_PROT_UNIT_TEST == 1
    // 原为静态函数，供单元测试使用
    uint16_t CalcCrc16(uint8_t* data, uint16_t len);
    void RawDataToEscapeData(uint8_t data[MAX_DATA_BUF_LEN], uint16_t* p_len);
    void EscapeDataToRawData(uint8_t data[MAX_DATA_BUF_LEN], uint16_t* p_len);
    uint16_t GetOnePackData(CommDevStruct* dev, const uint8_t* const input, uint32_t input_len, uint32_t* p_handle_len);
    bool IsPackCheckCorrect(uint8_t* pack_data, uint16_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif
