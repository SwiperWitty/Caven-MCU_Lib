#ifndef _UART_HANDLE__H_
#define _UART_HANDLE__H_

#include "base.h"
#include "Caven.h"

#define END_Data    '}'             //这个作为串口接收结束词，如果是 'N' 那么就是没有    RXD_Num_MAX
#define Length_MAX    Buff_Length   //Buff MAX 长度

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

extern struct _Uart_Data CV_UART[5];

void Uart_Init(char Channel, int Baud, int SET);

void UART_Send_String(char Channel, const char *String);
char UART_Send_Data(char Channel, const U8 *Data, int Length);


#endif
