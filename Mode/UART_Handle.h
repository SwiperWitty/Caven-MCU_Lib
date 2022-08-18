#ifndef _UART_HANDLE__H_
#define _UART_HANDLE__H_

#include "Base.h"
#include "Caven.h"
/*
        【API】      ————C语言环境即可运行



*/
#define END_Data    '}'             //这个作为串口接收【结束符】
#define NO_END      'N'             //如果【结束符】和它相同，那么就没有 结束符

#ifdef MCU_Buff_Len
    #define UART_Length_MAX    MCU_Buff_Len     //Buff MAX 长度(来自Items)
#else
    #define UART_Length_MAX    Buff_Length      //Buff MAX 长度(来自Caven)
#endif

struct _Uart_Data
{
    struct Caven_Data DATA;
    char Rxd_Received;
    U8 UART_RxdBuff[UART_Length_MAX];
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
