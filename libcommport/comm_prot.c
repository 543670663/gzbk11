#include "comm_prot.h"
#include "comm_prot_def.h"
#include <stdio.h>
#include <string.h>

#define HEAD_TAIL_ID        0xC0    // ��ͷ��β����
#define PACK_DATA_FLAG_IDX  4       // �����־λ�±�
#define PACK_DATA_LEN_IDX   6       // ���ݳ����±�

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
 * @brief        ��ԭʼ����ת��Ϊת������
 * @param data:  ����ʱΪԭʼ���ݣ����к���Ϊת�����������
 * @param p_len: ����ʱΪԭʼ���ݳ���ָ�룬���к���Ϊת�����ݳ���ָ�������
 * @attention    data�Ĵ�С����ڻ���� MAX_DATA_BUF_LEN
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
 * @brief        ��ת������ת��Ϊԭʼ����
 * @param data:  ����ʱΪת�����ݣ����к���Ϊԭʼ���������
 * @param p_len: ����ʱΪת�����ݳ���ָ�룬���к���Ϊԭʼ���ݳ���ָ�������
 * @attention    data�Ĵ�С����ڻ���� MAX_DATA_BUF_LEN
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
 * @brief ��������������У���ȡһ������
 * @param dev: ͨ���豸�ṹ��ָ��
 * @param input: �������ݣ����յ��Ĵ������ݣ�
 * @param len: �������ݳ���
 * @return ���ݰ�����: ��Ϊ��0ʱ��˵����ȡ����һ�����ݣ�Ϊ0��δ��ȡ��һ�����ݣ�
 * @note   �����ݺͻ������ݶ��� dev->bufData ��
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
            if (judge_byte == HEAD_TAIL_ID) { // ��ȡ����β
                uint16_t pack_len = *buf_len;
                *buf_len = 0;
                *got_head = true; // ���յ���β��Ҳ��Ϊ����һ���İ�ͷ
                //if (pack_len == 0) {
                //    // ����Ϊ0˵����ͷ��β������
                //    // ˵������һ������ͷ�������ϰ�����β
                //    *got_head = true;
                //}
                //else {
                //    *got_head = false;
                //}
                return pack_len;
            } else if ((*buf_len) < buf_max_len) { // ������ʱ��ֹͣ����
                buf_data[*buf_len] = judge_byte; // ��������
                (*buf_len)++;
            }
        } else {
            if (judge_byte == HEAD_TAIL_ID) { // ��ȡ����ͷ
                *buf_len = 0;
                *got_head = true;
            }
            // �ް�ͷʱ����������
        }
    }
    return 0;
}


/**
 * @name IsPackCheckCorrect
 * @brief ���������Ƿ���ȷ
 * @param pack_data: ������
 * @param len: ����
 * @return ���Ƿ���ȷ: falseΪ�����ݲ���ȷ��trueΪ��������ȷ
 */
_STATIC bool IsPackCheckCorrect(uint8_t *pack_data, uint16_t len)
{
    // �����ж�
    if (len < PACK_MIN_LEN) {
        COMM_LOG("������С��Ϊ%d\r\n", len);
        return false;
    } else if (len > PACK_MAX_LEN) {
        COMM_LOG("��������Ϊ%d\r\n", len);
        return false;
    }

    // CRC����
    FlagStruct *flag = (FlagStruct *)&pack_data[PACK_DATA_FLAG_IDX];
    if (flag->skipCrc16) {
        COMM_LOG("����CRCУ��\r\n");
    } else {
        uint16_t get_crc = ((uint16_t)pack_data[len - 1] << 8) + pack_data[len - 2];
        uint16_t calc_crc = CalcCrc16(pack_data, len - PACK_CRC_BYTE_NUM);

        if (get_crc != calc_crc) {
            COMM_LOG("CRCУ�����, ");
            COMM_LOG("Get CRC: 0x%04X, Calc CRC: 0x%04X\r\n", get_crc, calc_crc);
            return false;
        }
    }

    // ���ݳ����ж�
    uint8_t read_data_len = pack_data[PACK_DATA_LEN_IDX];
    if (read_data_len > PACK_DATA_MAX_LEN) {
        COMM_LOG("���ݳ���λ������Ϊ%d\r\n", read_data_len);
        return false;
    }
    uint16_t actual_data_len = len - PACK_CRC_BYTE_NUM - PACK_DATA_LEN_IDX - 1;
    if (read_data_len != actual_data_len) {
        COMM_LOG("���ݳ��Ȳ�һ��, ");
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
 * @brief ��ӡʼ��
 * @param print_func: ��ӡ����ָ�룬����NULL
 */
void CommPrintInit(PrintFptr print_func)
{
    if (print_func != NULL) {
        PrintFunc = print_func;
    }
}

/**
 * @name CommStructInit
 * @brief ͨ��Э���ʼ��
 * @param dev: ͨ���豸�ṹ��ָ��
 * @param cur_dev: ��ǰ�豸����
 * @param send_bytes_func: ���ݷ��ͺ���ָ�룬����NULL
 * @param recv_pack_callback: �����ջص�����ָ�룬����NULL
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
 * @brief ͨ��Э���ʼ��
 * @param dev: ͨ���豸�ṹ��ָ��
 * @param input: �������ݣ����յ��Ĵ������ݣ�
 * @param input_len: �������ݳ���
 * @note   ��������һ����Ч���ݺ��Զ�ִ�� dev->recvPackCallback
 */
void CommRecvData(CommDevStruct *dev, const uint8_t *const input, uint32_t input_len)
{
    if (input_len == 0) {
        return;
    }

    COMM_LOG("\r\n����������: ");
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

        COMM_LOG("ת������: ");
        for (uint32_t i = 0; i < pack_len; i++) {
            COMM_LOG("%02X ", buf[i]);
        }
        COMM_LOG("\r\n");
        // ������ת��
        EscapeDataToRawData(buf, &pack_len);
        COMM_LOG("ԭʼ����: ");
        for (uint32_t i = 0; i < pack_len; i++) {
            COMM_LOG("%02X ", buf[i]);
        }
        COMM_LOG("\r\n");

        // �Ƿ�У��ɹ�
        if (IsPackCheckCorrect(buf, pack_len) != true) {
            continue;
        }

        // CRC16��ֵ
        ((CommPackStruct *)buf)->crc16 = *(uint16_t *)(buf + pack_len - PACK_CRC_BYTE_NUM);

        // �����ص�
        dev->recvPackCallback((CommPackStruct *)buf);
    }
}

/**
 * @name CommSendData
 * @brief ��������
 * @param dev: ͨ���豸�ṹ��ָ��
 * @param target: ����Ŀ��
 * @param cmd: ����
 * @param data: ����
 * @param data_len: ���ݳ���
 * @param need_ack: �Ƿ���ҪӦ��
 * @return �Ƿ��ͳɹ�: true�ɹ���falseʧ��
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

    uint16_t crc16 = CalcCrc16(send, send_len - PACK_CRC_BYTE_NUM);//crcУ��
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
