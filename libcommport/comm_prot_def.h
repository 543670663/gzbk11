#ifndef __COMM_PROT_DEF_H
#define __COMM_PROT_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        COMM_W1 = 0x01,
        COMM_EC = 0x10,
        COMM_SM = 0x11,
        COMM_RECV_ALL = 0xFF,
    } DevIdEnum;

// 所有命令在这里定义


#ifdef __cplusplus
}
#endif

    
#endif
