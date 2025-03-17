#include "MODE_UART.h"


int MODE_UART_Init(char Channel,int Baud,int SET)
{
    int retavl = 1;
#ifdef Exist_UART
    Base_UART_Init((UART_mType)Channel,Baud,SET);
#endif
    return retavl;
}


void MODE_UART_Send_Data_Fun(char Channel, const U8 *Data, int Length)
{
#ifdef Exist_UART
	int temp = MIN(Length,UART_BUFF_MAX);
    int i = 0;

    while (temp--)
    {
        Base_UART_Send_Byte((UART_mType)Channel,Data[i++]);       //等待标志位在里面
    }
#endif
}

void MODE_UART_DMA_Send_Data_Fun(char Channel, const U8 *Data, int Length)
{
#ifdef Exist_UART
//    int temp = MIN(Length,UART_BUFF_MAX);

//    Base_UART_DMA_Send_Data((UART_mType)Channel,Data,temp);

#endif
}


void MODE_UART_Send_String_Fun(char Channel, const char *String)
{
    int Length = strlen(String);
    MODE_UART_Send_Data_Fun(Channel,(U8 *)String,Length);
}

void MODE_UART_Receive_Bind_Fun(char Channel, D_pFun UART_pFun)
{
#ifdef Exist_UART
    State_Machine_Bind ((UART_mType)Channel,UART_pFun);
#endif
}

