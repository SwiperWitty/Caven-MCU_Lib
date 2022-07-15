#include "uart_handle.h"

struct _Uart_Data CV_UART[5] = {0};

void Uart_Init(char Channel, int Baud,int SET)
{
#ifdef Exist_UART
    switch (Channel)
    {
    case 1:
        Uart1_Init(Baud,SET);
        break;
    case 2:
        Uart2_Init(Baud,SET);
        break;
    case 3:
        Uart3_Init(Baud,SET);
        break;
#ifdef UART_Channel_MAX
    #if UART_Channel_MAX >= 4
    case 4:
        Uart4_Init(Baud,SET);
        break;
    #endif
    #if UART_Channel_MAX >= 5
    case 5:
        Uart5_Init(Baud,SET);
        break;
    #endif
#endif

    default:
        break;
    }
#endif
}

#ifdef Exist_UART
static char Get_RXD(struct _Uart_Data *Target, char res) //接收处理函数
{
    if (!Target->Rxd_Received)      // 0 允许接收，其他不允许接收
    {
        Target->DATA.Buff[Target->DATA.Length++] = res;
        #if END_Data != NO_END      //如果有 停止符
        if (res == END_Data)    //Get停止符
        {
            Target->Rxd_Received = 1;
        }
        #endif

        #ifdef UART_Length_MAX
        if (Target->DATA.Length > UART_Length_MAX) //超长（异常需要清零）
        {
            Target->Rxd_Received = 2;
        }
        #endif
    }
    return Target->Rxd_Received;
}

void UART1_Interrupt()                         //              Interrupt
{
    uint8_t temp,Channel = 1;
    if(UART_RXD_Flag(Channel) != 0)
    {
        temp = UART_RXD_Receive(Channel);
        Get_RXD(&CV_UART[Channel], temp);
        UART_RXD_Flag_Clear(Channel);
//        UART_TXD_Send(Channel,temp);          //debug
    }
}
void UART2_Interrupt()
{
    uint8_t temp,Channel = 2;
    if(UART_RXD_Flag(Channel) != 0)
    {
        temp = UART_RXD_Receive(Channel);
        Get_RXD(&CV_UART[Channel], temp);
        UART_RXD_Flag_Clear(Channel);
//        UART_TXD_Send(Channel,temp);          //debug
    }
}
void UART3_Interrupt()
{
    uint8_t temp,Channel = 3;
    if(UART_RXD_Flag(Channel) != 0)
    {
        temp = UART_RXD_Receive(Channel);
        Get_RXD(&CV_UART[Channel], temp);
        UART_RXD_Flag_Clear(Channel);
//        UART_TXD_Send(Channel,temp);          //debug
    }
}
#ifdef UART_Channel_MAX
    #if UART_Channel_MAX >= 4
    void UART4_Interrupt()
    {
        uint8_t temp,Channel = 4;
        if(UART_RXD_Flag(Channel) != 0)
        {
            temp = UART_RXD_Receive(Channel);
            Get_RXD(&CV_UART[Channel], temp);
            UART_RXD_Flag_Clear(Channel);
        }
    }
    #endif
    #if UART_Channel_MAX >= 5
    void UART5_Interrupt()
    {
        uint8_t temp,Channel = 4;
        if(UART_RXD_Flag(Channel) != 0)
        {
            temp = UART_RXD_Receive(Channel);
            Get_RXD(&CV_UART[Channel], temp);
            UART_RXD_Flag_Clear(Channel);
        }
    }
    #endif
#endif

#endif

char UART_Send_Data(char Channel, const U8 *Data, int Length)
{
#ifdef Exist_UART
    if (Length > UART_Length_MAX)
        return 0;
	if (Channel > UART_Channel_MAX)
		return (char)-1;

    int temp = Length;
    int i = 0;
    while (temp--)
    {
        UART_TXD_Send(Channel,Data[i++]);
    }
#endif
    return 1;
}

void UART_Send_String(char Channel, const char *String)
{
    int Length = strlen(String);
	UART_Send_Data(Channel,(U8 *)String,Length);
}

