#ifndef _UART_HANDLE__H_
#define _UART_HANDLE__H_

#include "Base.h"
#include "Caven.h"
/*
        【API】      ————C语言环境即可运行
    判断串口结束的 define被丢到底层了(很烦)        ———— 2022.8.18


*/

#ifdef MCU_Buff_Len
    #define UART_Length_MAX    MCU_Buff_Len     //Buff MAX 长度(来自Items)
#else
    #define UART_Length_MAX    Buff_Length      //Buff MAX 长度(来自Caven)
#endif

#define END_Data    '}'             //这个作为串口接收【结束符】
#define NO_END      'N'             //如果【结束符】和它相同，那么就没有 结束符


struct _Uart_Data
{
    struct Caven_Data DATA;
    char Rxd_Received;
    U8 UART_RxdBuff[UART_Length_MAX];       //串口数据在这，大小由MCU的底层决定
};

struct Uart_
{
    void (*Send_String)(char Channel, const char *String);
    char (*Send_Data)(char Channel, const U8 *Data, int Length);
};

extern struct _Uart_Data * CV_UART[];       //别的地方定义数量


void Uart_Init(char Channel, int Baud, int SET);

void UART_Send_String(char Channel, const char *String);
char UART_Send_Data(char Channel, const U8 *Data, int Length);


#endif
