#ifndef __BASE_CAN__H_
#define __BASE_CAN__H_

#if DEFAULT
#include "Items.h" // 默认功能
#include "Caven_Type.h"
#else
#define Exist_CAN   1
#define CAN_MODE    1
typedef void (*D_pFun) (void *data);
#include "ch32v30x_can.h"

typedef unsigned char u8;
typedef unsigned short u16;
#endif


#if Exist_CAN
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

#define USE_SOFT_FILTER                 1
#define CANSOFTFILTER_MAX_GROUP_NUM     14 // The maximum recommended configuration is 14.
#define CANSOFTFILER_PREDEF_CTRLBYTE_MASK32 ((CAN_FilterScale_32bit << 5) | (CAN_FilterMode_IdMask << 1))
#define CANSOFTFILER_PREDEF_CTRLBYTE_ID32 ((CAN_FilterScale_32bit << 5) | (CAN_FilterMode_IdList << 1))

#define CAN_SEND_OVERTIME   0Xffff

#define CAN_SJW_H_NUM CAN_SJW_1tq
#define CAN_SJW_L_NUM CAN_SJW_3tq

#define CAN_BS1_NUM CAN_BS1_6tq
#define CAN_BS2_NUM CAN_BS2_5tq

#define CAN_NORMAL_MODE             CAN_Mode_Normal
#define CAN_SILENT_MODE             CAN_Mode_Silent
#define CAN_LOOPBACK_MODE           CAN_Mode_LoopBack
#define CAN_SILENT_LOOPBACK_MODE    CAN_Mode_Silent_LoopBack

#endif

// fun
int Base_CAN_Init(u8 tsjw,u8 mode,int Bps,int Id,int en);
int Base_CAN_Filter (int *Filter,int len,int Range_a,int Range_b);
int Base_CAN_Send_Data(u8 *Data, u16 len);
int Base_CAN_Get_RXId (void);

/*
 * 接收入口的状态机，这个很重要
 * 在初始化之后执行一次
 * 不可用阻塞
 */
int CAN_Receive_Bind_Fun(D_pFun Data_pFun);

#endif
