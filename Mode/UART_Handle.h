#ifndef _UART_HANDLE__H_
#define _UART_HANDLE__H_

#include "Base.h"
#include "Caven.h"
/*
        【API】      ————C语言环境即可运行
    必读：
    UART的数据是结构体数组，[0]是备用缓存区（可以作为任意串口之一的备份），所以串口1的数据区在CV_UART[1]，以此类推。
    !注意!CV_UART.DATA.Buff的长度，这很重要!

*/
#define END_Data    'N'             //这个作为串口接收【结束符】
#define NO_END      'N'             //如果【结束符】和它相同，那么就没有 结束符

#define UART_Length_MAX    Buff_Length   //Buff MAX 长度(来自Caven)

struct _Uart_Data
{
    struct Caven_Data DATA;
    volatile char Rxd_Received;
};

struct Uart_
{
    void (*Send_String)(char Channel, const char *String);
    char (*Send_Data)(char Channel, const U8 *Data, int Length);
};

#ifdef Exist_UART
    extern struct _Uart_Data CV_UART[UART_Channel_MAX];
#endif

void Uart_Init(char Channel, int Baud, int SET);

void UART_Send_String(char Channel, const char *String);
char UART_Send_Data(char Channel, const U8 *Data, int Length);


#endif
