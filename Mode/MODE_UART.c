#include "MODE_UART.h"


int MODE_UART_Init(char Channel,int Baud,int Set)
{
    int retavl = 1;
#if Exist_UART
    Base_UART_Init((UART_mType)Channel,Baud,SET);
#endif
    return retavl;
}


void MODE_UART_Send_Data_Fun(char Channel, const U8 *Data, int Length)
{
#if Exist_UART
    int i = 0;
    while (Length-- > 0)
    {
        Base_UART_Send_Data((UART_mType)Channel,Data[i++]);       //等待标志位在里面
    }
#endif
}

void MODE_UART_DMA_Send_Data_Fun(char Channel, const U8 *Data, int Length)
{
    int temp = Length;
#ifdef DMA_UART
    temp = MIN(Length,UART_BUFF_MAX);
    Base_UART_DMA_Send_Data((UART_mType)Channel,Data,temp);
#else
    MODE_UART_Send_Data_Fun(Channel,(U8 *)String,temp);
#endif
}


void MODE_UART_Send_String_Fun(char Channel, const char *String)
{
    int Length = strlen(String);
    MODE_UART_Send_Data_Fun(Channel,(U8 *)String,Length);
}

void MODE_UART_Receive_Bind_Fun(char Channel, D_pFun UART_pFun)
{
#if Exist_UART
    State_Machine_Bind ((UART_mType)Channel,UART_pFun);
#endif
}

