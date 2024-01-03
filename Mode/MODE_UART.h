#ifndef _UART_HANDLE__H_
#define _UART_HANDLE__H_

#include "Base.h"
#include "API.h"

/*
        【API】      ————C语言环境即可运行
    判断串口结束的 define被丢到底层了(很烦)        ———— 2022.8.18

    //没数据？ 看看 END_Data
    // DEBUG_RX_TX 看通不通
*/

#ifdef MCU_Buff_Len
    #define UART_Length_MAX    MCU_Buff_Len     //Buff MAX 长度(来自Items & User_items)
#else
    #define UART_Length_MAX    BUFF_MAX         //Buff MAX 长度(来自Caven_Type)
#endif

#define DEBUG_RX_BK_TX  0   // 0:off 1:on 


typedef struct
{
    void (*Send_String_pFun)(UART_mType Channel, const char *String);
    void (*Send_Data_pFun)(UART_mType Channel, const U8 *Data, int Length);
    
    void (*Receive_Bind_pFun)(UART_mType Channel, D_pFun UART_pFun);
}MODE_UART_Way;


int MODE_UART_Init(UART_mType Channel,int Baud,int SET);


void MODE_UART_Send_Data_Fun(UART_mType Channel, const U8 *Data, int Length);
void MODE_UART_DMA_Send_Data_Fun(UART_mType Channel, const U8 *Data, int Length);

void MODE_UART_Send_String_Fun(UART_mType Channel, const char *String);

void MODE_UART_Receive_Bind_Fun(UART_mType Channel, D_pFun UART_pFun);

#endif
