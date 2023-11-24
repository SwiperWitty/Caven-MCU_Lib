#include "Base_UART.h"



#ifdef Exist_UART

#define RXD_Falg    USART_IT_RXNE       //  接收标志
#define TXD_Falg    USART_FLAG_TC       //  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

static D_pFun State_Machine_UART1_pFun = NULL;
static D_pFun State_Machine_UART2_pFun = NULL;
static D_pFun State_Machine_UART3_pFun = NULL;

// 以下函数单纯方便MCU移植

static char UART_RXD_Flag(UART_mType Channel)
{
    char res;
    USART_TypeDef * Temp;
    switch (Channel)
    {
    case 0:
        return 0;
    case 1:
        Temp = USART1;
        break;
    case 2:
        Temp = USART2;
        break;
    case 3:
        Temp = USART3;
        break;
    default:
        return 0;
    }
    res = USART_GetITStatus(Temp,RXD_Falg);
    return res;
}

static void UART_RXD_Flag_Clear(UART_mType Channel)
{
    USART_TypeDef * Temp;
    switch (Channel)
    {
    case 0:
        return;
    case 1:
        Temp = USART1;
        break;
    case 2:
        Temp = USART2;
        break;
    case 3:
        Temp = USART3;
        break;
    default:
        return;
    }
    USART_ClearFlag(Temp, RXD_Falg);
}

static uint16_t UART_RXD_Receive(UART_mType Channel)     //RXD 读取值
{
    uint16_t res;
    USART_TypeDef * Temp;
    switch (Channel)
    {
    case 0:
        return 0;
    case 1:
        Temp = USART1;
        break;
    case 2:
        Temp = USART2;
        break;
    case 3:
        Temp = USART3;
        break;
    default:
        return 0;
    }
    res = USART_ReceiveData(Temp);
    return res;
}

void Base_UART_Send_Byte(UART_mType Channel,uint16_t DATA)
{
    USART_TypeDef * Temp;
    switch (Channel)
    {
    case 0:
        return;
    case 1:
        Temp = USART1;
        break;
    case 2:
        Temp = USART2;
        break;
    case 3:
        Temp = USART3;
        break;
    default:
        return;
    }
    while (USART_GetFlagStatus(Temp, TXD_Falg) == 0);
    USART_ClearFlag(Temp, TXD_Falg);
    USART_SendData(Temp, DATA);
}
#endif

// 以下函数很重要，包括功能启动，功能中断处理

#if (Exist_UART & OPEN_0001)
void Uart0_Init(int Baud,int SET)
{

}
#endif

#if (Exist_UART & OPEN_0010)
void Uart1_Init(int Baud,int SET)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
	USART_InitTypeDef USART_InitStructure = {0};
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	USART_TypeDef * UART_Temp = USART1;
	FunctionalState temp;

    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, temp);	  // USART1  (APB2)
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟(暂不需要)
//  GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);     //端口复用
    USART_DeInit(UART_Temp);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;      // RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;       // TXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					//
    USART_InitStructure.USART_Parity = USART_Parity_No;								//
    USART_InitStructure.USART_StopBits = USART_StopBits_1;							//
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//
    USART_Init(UART_Temp, &USART_InitStructure);
    USART_ITConfig(UART_Temp, RXD_Falg, temp);                                 //

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = temp;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(UART_Temp, temp);
}

void UART1_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH1;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART1_pFun != NULL) {
            State_Machine_UART1_pFun(temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_0100)
void Uart2_Init(int Baud,int SET)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    FunctionalState temp;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //  PA02 PA03
    RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, temp);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		// USART2 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);				  //端口A；
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;			  // USART2 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
    GPIO_Init(GPIOA, &GPIO_InitStructure);				  //端口A；
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = Baud;					//波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);		//配置串口参数
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置中断组，4位抢占优先级，4位响应优先级；
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		  //中断号；
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级；
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, temp); //使能串口
}

void UART2_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH2;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART2_pFun != NULL) {
            State_Machine_UART2_pFun(temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_1000)
void Uart3_Init(int Baud,int SET)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    
    FunctionalState temp;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
    
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 //设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   // PC10 PC11
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, temp);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE); //串口3重映射，与串口4 IO一致（STM32F103RB版本只有串口3没有4）

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		// USART3 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			  // USART3 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = Baud;					//波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);				  //配置串口参数
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			  //设置中断组，4位抢占优先级，4位响应优先级；
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		  //中断号；
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级；
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //响应优先级；
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, temp);
}

void UART3_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH3;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART3_pFun != NULL) {
            State_Machine_UART3_pFun(temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_0001)
void Uart4_Init(int Baud,int SET)
{


}

void UART4_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH4;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART4_pFun != NULL) {
            State_Machine_UART4_pFun(temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif


int Base_UART_Init(UART_mType Channel,int Baud,int SET)
{
    int retval = -1;
#ifdef Exist_UART
    switch (Channel)
    {
    case 0:
        break;
    case 1:
        Uart1_Init(Baud,SET);
        retval = 0;
        break;
    case 2:
        Uart2_Init(Baud,SET);
        retval = 0;
        break;
    case 3:
        Uart3_Init(Baud,SET);
        retval = 0;
        break;
    default:
        break;
    }
#endif
    return retval;
}

/*
 *  Successful : return 0
 *
 */
int State_Machine_Bind (UART_mType Channel,D_pFun UART_pFun)
{
    int retval = -1;
#ifdef Exist_UART
    if (UART_pFun == NULL)
    {
        return retval;
    }
    switch (Channel)
    {
    case 0:
//        State_Machine_UART0_pFun = UART_pFun;
        break;
    case 1:
        State_Machine_UART1_pFun = UART_pFun;
        retval = 0;
        break;
    case 2:
        State_Machine_UART2_pFun = UART_pFun;
        retval = 0;
        break;
    case 3:
        State_Machine_UART3_pFun = UART_pFun;
        retval = 0;
        break;
    default:
        break;
    }
#endif
    return retval;
}
