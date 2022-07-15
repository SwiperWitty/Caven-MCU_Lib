#ifndef _UART_HANDLE__H_
#define _UART_HANDLE__H_

#include "base.h"
#include "Caven.h"
/*
        【API】      ————C语言环境即可运行



*/
#define END_Data    '}'             //这个作为串口接收【结束符】
#define NO_END      'N'             //如果【结束符】和它相同，那么就没有 结束符

#define UART_Length_MAX    Buff_Length   //Buff MAX 长度(来自Caven)

struct _Uart_Data
{
    struct Caven_Data DATA;
    char Rxd_Received;
};

struct Uart_
{
    void (*Send_String)(char Channel, const char *String);
    char (*Send_Data)(char Channel, const U8 *Data, int Length);
};

#ifdef Exist_UART
    extern struct _Uart_Data CV_UART[5];
#endif

void Uart_Init(char Channel, int Baud, int SET);

void UART_Send_String(char Channel, const char *String);
char UART_Send_Data(char Channel, const U8 *Data, int Length);


#endif
