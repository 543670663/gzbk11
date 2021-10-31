#include "comm_prot.h"
#include "comm_prot_def.h"
#include <stdio.h>
#include <string.h>

#define HEAD_TAIL_ID        0xC0    // 包头包尾定义
#define PACK_DATA_FLAG_IDX  4       // 命令标志位下标
#define PACK_DATA_LEN_IDX   6       // 数据长度下标

#define ESC_HEX             0xDB
#define ESC_HEAD_HEX        0xDC
#define ESC_ESC_HEX         0xDD

#if COMM_PROT_UNIT_TEST == 1
    #define _STATIC
#else
    #define _STATIC static
#endif

static void Print(const char *const str)
{
    (void)str;
}

static PrintFptr PrintFunc = Print;

_STATIC uint16_t CalcCrc16(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint16_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**
 * @name         RawDataToEscapeData
 * @brief        将原始数据转换为转义数据
 * @param data:  传参时为原始数据，运行后作为转义数据输出；
 * @param p_len: 传参时为原始数据长度指针，运行后作为转义数据长度指针输出；
 * @attention    data的大小需大于或等于 MAX_DATA_BUF_LEN
 */
_STATIC void RawDataToEscapeData(uint8_t data[MAX_DATA_BUF_LEN], uint16_t *p_len)
{
    if (*p_len > PACK_MAX_LEN) {
        return;
    }
    uint16_t handle_len = (*p_len);
    uint16_t escape_len = 0;
    uint8_t backup[MAX_DATA_BUF_LEN] = { 0 };
    memcpy(backup, data, handle_len);

    for (uint16_t i = 0; i < handle_len; i++) {
        if (backup[i] == HEAD_TAIL_ID) {
            data[escape_len]     = ESC_HEX;
            data[escape_len + 1] = ESC_HEAD_HEX;
            escape_len += 2;
        } else if (backup[i] == ESC_HEX) {
            data[escape_len] = ESC_HEX;
            data[escape_len + 1] = ESC_ESC_HEX;
            escape_len += 2;
        } else {
            data[escape_len] = backup[i];
            escape_len += 1;
        }
    }
    *p_len = escape_len;
}

/**
 * @name         EscapeDataToRawData
 * @brief        将转义数据转换为原始数据
 * @param data:  传参时为转义数据，运行后作为原始数据输出；
 * @param p_len: 传参时为转义数据长度指针，运行后作为原始数据长度指针输出；
 * @attention    data的大小需大于或等于 MAX_DATA_BUF_LEN
 */
_STATIC void EscapeDataToRawData(uint8_t data[MAX_DATA_BUF_LEN], uint16_t *p_len)
{
    if (*p_len > MAX_DATA_BUF_LEN) {
        return;
    }
    uint16_t handle_len = (*p_len);
    uint16_t raw_data_len = 0;
    uint8_t backup[MAX_DATA_BUF_LEN] = { 0 };
    memcpy(backup, data, handle_len);

    for (uint16_t i = 0; i < handle_len; i++) {
        if (backup[i] == ESC_HEX && backup[i + 1] == ESC_HEAD_HEX) {
            data[raw_data_len] = HEAD_TAIL_ID;
            i++;
        } else if (backup[i] == ESC_HEX && backup[i + 1] == ESC_ESC_HEX) {
            data[raw_data_len] = ESC_HEX;
            i++;
        } else {
            data[raw_data_len] = backup[i];
        }
        raw_data_len++;
    }
    *p_len = raw_data_len;
}

/**
 * @name GetOnePackData
 * @brief 从输入的数据流中，获取一包数据
 * @param dev: 通信设备结构体指针
 * @param input: 输入数据（接收到的串口数据）
 * @param len: 输入数据长度
 * @return 数据包长度: 当为非0时，说明获取到了一包数据；为0则还未获取到一包数据；
 * @note   包数据和缓存数据都在 dev->bufData 中
 */
_STATIC uint16_t GetOnePackData(CommDevStruct *dev, const uint8_t *const input, uint32_t input_len, uint32_t *p_handle_len)
{
    bool *got_head = &dev->gotHead;
    uint16_t *buf_len = &dev->bufLen;
    uint8_t *buf_data = dev->bufData;
    uint16_t buf_max_len = sizeof(dev->bufData);

    while ((*p_handle_len) < input_len) {
        uint8_t judge_byte = input[*p_handle_len];
        (*p_handle_len)++;
        if (*got_head) {
            if (judge_byte == HEAD_TAIL_ID) { // 获取到包尾
                uint16_t pack_len = *buf_len;
                *buf_len = 0;
                *got_head = true; // 接收到包尾，也认为是下一包的包头
                //if (pack_len == 0) {
                //    // 长度为0说明包头包尾紧挨着
                //    // 说明是上一包丢了头，或上上包丢了尾
                //    *got_head = true;
                //}
                //else {
                //    *got_head = false;
                //}
                return pack_len;
            } else if ((*buf_len) < buf_max_len) { // 数据满时，停止缓存
                buf_data[*buf_len] = judge_byte; // 缓存数据
                (*buf_len)++;
            }
        } else {
            if (judge_byte == HEAD_TAIL_ID) { // 获取到包头
                *buf_len = 0;
                *got_head = true;
            }
            // 无包头时，抛弃数据
        }
    }
    return 0;
}


/**
 * @name IsPackCheckCorrect
 * @brief 检查包数据是否正确
 * @param pack_data: 包数据
 * @param len: 包长
 * @return 包是否正确: false为包数据不正确，true为包数据正确
 */
_STATIC bool IsPackCheckCorrect(uint8_t *pack_data, uint16_t len)
{
    // 包长判断
    if (len < PACK_MIN_LEN) {
        COMM_LOG("包长过小，为%d\r\n", len);
        return false;
    } else if (len > PACK_MAX_LEN) {
        COMM_LOG("包长过大，为%d\r\n", len);
        return false;
    }

    // CRC检验
    FlagStruct *flag = (FlagStruct *)&pack_data[PACK_DATA_FLAG_IDX];
    if (flag->skipCrc16) {
        COMM_LOG("跳过CRC校验\r\n");
    } else {
        uint16_t get_crc = ((uint16_t)pack_data[len - 1] << 8) + pack_data[len - 2];
        uint16_t calc_crc = CalcCrc16(pack_data, len - PACK_CRC_BYTE_NUM);

        if (get_crc != calc_crc) {
            COMM_LOG("CRC校验错误, ");
            COMM_LOG("Get CRC: 0x%04X, Calc CRC: 0x%04X\r\n", get_crc, calc_crc);
            return false;
        }
    }

    // 数据长度判断
    uint8_t read_data_len = pack_data[PACK_DATA_LEN_IDX];
    if (read_data_len > PACK_DATA_MAX_LEN) {
        COMM_LOG("数据长度位过长，为%d\r\n", read_data_len);
        return false;
    }
    uint16_t actual_data_len = len - PACK_CRC_BYTE_NUM - PACK_DATA_LEN_IDX - 1;
    if (read_data_len != actual_data_len) {
        COMM_LOG("数据长度不一致, ");
        COMM_LOG("Get Len: %d, Read Len: %d\r\n", actual_data_len, read_data_len);
        return false;
    }
    return true;
}

static uint32_t SendBytes(uint8_t *bytes, uint32_t len)
{
    (void)bytes;
    return len;
}

static void RecvPackCallback(CommPackStruct *pack)
{
    pack = (void *)pack;
}

/**
 * @name CommStructInit
 * @brief 打印始化
 * @param print_func: 打印函数指针，允许传NULL
 */
void CommPrintInit(PrintFptr print_func)
{
    if (print_func != NULL) {
        PrintFunc = print_func;
    }
}

/**
 * @name CommStructInit
 * @brief 通信协议初始化
 * @param dev: 通信设备结构体指针
 * @param cur_dev: 当前设备定义
 * @param send_bytes_func: 数据发送函数指针，允许传NULL
 * @param recv_pack_callback: 包接收回调函数指针，允许传NULL
 */
void CommStructInit(CommDevStruct *dev, uint8_t cur_dev,
                    SendBytesFptr send_bytes_func,
                    RecvPackCallbackFptr recv_pack_callback)
{
    memset(dev, 0, sizeof(CommDevStruct));
    dev->currentDevice = (DevIdEnum)cur_dev;
    dev->gotHead = false;
    if (send_bytes_func == NULL) {
        dev->sendByteFunc = SendBytes;
    } else {
        dev->sendByteFunc = send_bytes_func;
    }
    if (recv_pack_callback == NULL) {
        dev->recvPackCallback = RecvPackCallback;
    } else {
        dev->recvPackCallback = recv_pack_callback;
    }
}

/**
 * @name CommStructInit
 * @brief 通信协议初始化
 * @param dev: 通信设备结构体指针
 * @param input: 输入数据（接收到的串口数据）
 * @param input_len: 输入数据长度
 * @note   当解析到一包有效数据后，自动执行 dev->recvPackCallback
 */
void CommRecvData(CommDevStruct *dev, const uint8_t *const input, uint32_t input_len)
{
    if (input_len == 0) {
        return;
    }

    COMM_LOG("\r\n待处理数据: ");
    for (uint32_t i = 0; i < input_len; i++) {
        COMM_LOG("%02X ", input[i]);
    }
    COMM_LOG("\r\n");

    uint32_t handle_len = 0;
    uint8_t *buf = dev->bufData;

    while (handle_len < input_len) {
        uint16_t pack_len = GetOnePackData(dev, input, input_len, &handle_len);
        if (pack_len == 0) {
            continue;
        }

        COMM_LOG("转义数据: ");
        for (uint32_t i = 0; i < pack_len; i++) {
            COMM_LOG("%02X ", buf[i]);
        }
        COMM_LOG("\r\n");
        // 包数据转义
        EscapeDataToRawData(buf, &pack_len);
        COMM_LOG("原始数据: ");
        for (uint32_t i = 0; i < pack_len; i++) {
            COMM_LOG("%02X ", buf[i]);
        }
        COMM_LOG("\r\n");

        // 是否校验成功
        if (IsPackCheckCorrect(buf, pack_len) != true) {
            continue;
        }

        // CRC16赋值
        ((CommPackStruct *)buf)->crc16 = *(uint16_t *)(buf + pack_len - PACK_CRC_BYTE_NUM);

        // 触发回调
        dev->recvPackCallback((CommPackStruct *)buf);
    }
}

/**
 * @name CommSendData
 * @brief 发送数据
 * @param dev: 通信设备结构体指针
 * @param target: 接收目标
 * @param cmd: 命令
 * @param data: 数据
 * @param data_len: 数据长度
 * @param need_ack: 是否需要应答
 * @return 是否发送成功: true成功，false失败
 */
bool CommSendData(CommDevStruct *dev, uint8_t target, uint16_t cmd, const uint8_t *const data, uint8_t data_len, bool need_ack)
{
    CommPackStruct pack = { 0 };
    pack.sender = dev->currentDevice;
    pack.receiver = target;
    pack.cmd = cmd;
    pack.flag.needAck = need_ack;
    pack.timestamp = dev->timestamp;
    pack.dataLen = data_len;
    memcpy(pack.data, data, pack.dataLen);

    uint8_t *send = (uint8_t *)&pack;
    uint16_t send_len = pack.dataLen + PACK_MIN_LEN;

    uint16_t crc16 = CalcCrc16(send, send_len - PACK_CRC_BYTE_NUM);//crc校验
    memcpy(pack.data + pack.dataLen, &crc16, sizeof(crc16));
    COMM_LOG("send: ");
    for (uint32_t i = 0; i < send_len; i ++) {
        COMM_LOG("%02X ", send[i]);
    }
    COMM_LOG("\r\n");
    RawDataToEscapeData(send, &send_len);

    uint8_t tmp = HEAD_TAIL_ID;
    if (dev->sendByteFunc(&tmp, 1) != 1) {
        return false;
    }
    if (dev->sendByteFunc(send, send_len) != send_len) {
        return false;
    }
    if (dev->sendByteFunc(&tmp, 1) != 1) {
        return false;
    }
    return true;
}
