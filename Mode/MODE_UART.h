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



typedef struct
{
    void (*Send_String_pFun)(char Channel, const char *String);
    void (*Send_Data_pFun)(char Channel, const U8 *Data, int Length);
    
    void (*Receive_Bind_pFun)(char Channel, D_pFun UART_pFun);
}MODE_UART_Way;


int MODE_UART_Init(char Channel,int Baud,int SET);


void MODE_UART_Send_Data_Fun(char Channel, const U8 *Data, int Length);
void MODE_UART_DMA_Send_Data_Fun(char Channel, const U8 *Data, int Length);

void MODE_UART_Send_String_Fun(char Channel, const char *String);

void MODE_UART_Receive_Bind_Fun(char Channel, D_pFun UART_pFun);

#endif
