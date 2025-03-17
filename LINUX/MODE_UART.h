#ifndef _UART_HANDLE__H_
#define _UART_HANDLE__H_

#include "Items.h"
#include "API.h"

/*
        【API】      ————C语言环境即可运行
    判断串口结束的 define被丢到底层了(很烦)        ———— 2022.8.18
    适配linux               ———— 2024.12.20

    1).默认已经使用了函数MODE_UART_Init(没使用或者Disable,此通道不执行发送和接收)
    2).发送是操作文件的型式发送[MODE_UART_Send_Data_Fun]，和MCU类似。
    3).接收有两种方式，一种是[MODE_UART_Receive_Get_Fun]，他是非阻塞接收的，也就是说他需要进线程才能收到数据
    另一种是开一个多线程任务去阻塞接收，当然你也可以在此线程中使用第一种方式，如果要阻塞接收需使用[MODE_UART_Receive_Bind_Fun]
    绑定一个数据接收机(状态机)[D_pFun]。推荐使用线程，因为这样可以不进主线处理数据，这与MCU的中断处理逻辑不谋而合。
    ——2025/1/17 
*/


typedef struct
{
    void (*Send_String_pFun)(char Channel, const char *String);
    void (*Send_Data_pFun)(char Channel, const U8 *Data, int Length);
    
    void (*Receive_Bind_pFun)(char Channel, D_pFun UART_pFun);
    int (*Receive_Get_Fun)(char Channel,uint8_t* Data,int Length);
}MODE_UART_Way;


int MODE_UART_Init(char Channel,int Baud,int SET);


void MODE_UART_Send_Data_Fun(char Channel, const U8 *Data, int Length);
void MODE_UART_DMA_Send_Data_Fun(char Channel, const U8 *Data, int Length);

void MODE_UART_Send_String_Fun(char Channel, const char *String);

void MODE_UART_Receive_Bind_Fun(char Channel, D_pFun UART_pFun);
int MODE_UART_Receive_Get_Fun(char Channel,uint8_t* Data,int Length);

#endif
