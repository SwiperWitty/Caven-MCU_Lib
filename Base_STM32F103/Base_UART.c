#include "Base_UART.h" 

#ifdef Exist_UART

#define RXD_Falg    USART_IT_RXNE     //  接收标志
#define TXD_Falg    USART_FLAG_TC		//  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

static USART_TypeDef * uart_Temp;
static D_pFun State_Machine_UART_pFun[6];

uint8_t uart0_enable = 0;
uint8_t uart1_enable = 0;
uint8_t uart2_enable = 0;
uint8_t uart3_enable = 0;
uint8_t uart4_enable = 0;
uint8_t uart5_enable = 0;

static char UART_RXD_Flag(UART_mType Channel)
{
    char res;
    switch (Channel)
    {
    case 0:
        return 0;
    case 1:
        uart_Temp = USART1;
        break;
    case 2:
        uart_Temp = USART2;
        break;
    case 3:
        uart_Temp = USART3;
        break;
    case 4:
        uart_Temp = UART4;
        break;
    default:
        return 0;
    }
    res = USART_GetITStatus(uart_Temp,RXD_Falg);
    return res;
}

static void UART_RXD_Flag_Clear(UART_mType Channel)
{
    switch (Channel)
    {
    case 0:
        return;
    case 1:
        uart_Temp = USART1;
        break;
    case 2:
        uart_Temp = USART2;
        break;
    case 3:
        uart_Temp = USART3;
        break;
    case 4:
        uart_Temp = UART4;
        break;
    default:
        return;
    }
    USART_ClearFlag(uart_Temp, RXD_Falg);
}

/*  发送 接收    */

// 接收
static uint16_t UART_RXD_Receive(UART_mType Channel)     //RXD 读取值
{
    uint16_t res;
    switch (Channel)
    {
    case 0:
        return 0;
    case 1:
        uart_Temp = USART1;
        break;
    case 2:
        uart_Temp = USART2;
        break;
    case 3:
        uart_Temp = USART3;
        break;
    case 4:
        uart_Temp = UART4;
        break;
    default:
        return 0;
    }
    res = USART_ReceiveData(uart_Temp);
    return res;
}

// 发送
void Base_UART_Send_Byte(UART_mType Channel,uint16_t DATA)
{
    switch (Channel)
    {
    case 0:
        if (uart0_enable == 0)
        {
            return;
        }
        return;
    case 1:
        if (uart1_enable == 0)
        {
            return;
        }
        uart_Temp = USART1;
        break;
    case 2:
        if (uart2_enable == 0)
        {
            return;
        }
        uart_Temp = USART2;
        break;
    case 3:
        if (uart3_enable == 0)
        {
            return;
        }
        uart_Temp = USART3;
        break;
    case 4:
        if (uart4_enable == 0)
        {
            return;
        }
        uart_Temp = UART4;
        break;
    default:
        return;
    }
    
    while (USART_GetFlagStatus(uart_Temp, TXD_Falg) == 0);
    USART_ClearFlag(uart_Temp, TXD_Falg);
    USART_SendData(uart_Temp, DATA);
}

void Base_UART_DMA_Send_Data(UART_mType Channel,const uint8_t *DATA,int Length)
{
    
}

#endif

#if (Exist_UART & OPEN_0001)
void Uart0_Init(int Baud,int SET)
{
    uart0_enable = SET;
}

void UART0_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH0;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}
#endif

#if (Exist_UART & OPEN_0010)
void Uart1_Init(int Baud,int Set)
{
    FunctionalState Cmd_set;
    uart_Temp = USART1;
    USART_DeInit(uart_Temp);
    uart1_enable = SET;
    if (Set) {Cmd_set = ENABLE;}
    else {Cmd_set = DISABLE;}
    
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; //
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		//设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    #if 1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    #else
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);		//串口1重映射
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TXD
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	  //RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    #endif
    /***/
    USART_InitStructure.USART_BaudRate = Baud; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//
    USART_Init(uart_Temp, &USART_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(uart_Temp, USART_IT_RXNE, Cmd_set);   //
    USART_Cmd(uart_Temp, Cmd_set);					    //
}

void UART1_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH1;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_0100)
void Uart2_Init(int Baud,int Set)
{
    FunctionalState set = DISABLE;
    uart_Temp = USART2;
    USART_DeInit(uart_Temp);
    uart2_enable = SET;
    if (Set)
        set = ENABLE;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; //
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		//设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//AFIO复用功能模块时钟

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, set);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //USART2 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出；
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); //端口A；
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //USART2 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入；
    GPIO_Init(GPIOA, &GPIO_InitStructure); //端口A；
    
    /***/
    USART_InitStructure.USART_BaudRate = Baud; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//
    USART_Init(uart_Temp, &USART_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(uart_Temp, USART_IT_RXNE, set);   //
    USART_Cmd(uart_Temp, set);					    //
}

void UART2_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH2;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_1000)
void Uart3_Init(int Baud,int Set)
{
    FunctionalState set = DISABLE;
    uart_Temp = USART3;
    USART_DeInit(uart_Temp);
    uart3_enable = SET;
    if (Set)
        set = ENABLE;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; //
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		//设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//AFIO复用功能模块时钟
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    #if 1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //USART3 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //USART3 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    #else
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);		//串口3重映射，与串口4 IO一致（STM32F103RB版本只有串口3没有4）
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //USART3 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //USART3 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    #endif
    /***/
    USART_InitStructure.USART_BaudRate = Baud; //波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; //无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//
    USART_Init(uart_Temp, &USART_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(uart_Temp, USART_IT_RXNE, set);   //
    USART_Cmd(uart_Temp, set);					    //
}

void UART3_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH3;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_10000)
void Uart4_Init(int Baud,int SET)
{
    uart4_enable = SET;

}

void UART4_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH4;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

int Base_UART_Init(UART_mType Channel, int Baud, int SET)
{
    int retval = -1;
	
#ifdef Exist_UART
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    switch (Channel)
    {
    case 0:
		
        break;
    case 1:
        Uart1_Init(Baud, SET);
        retval = 0;
        break;
    case 2:
        Uart2_Init(Baud, SET);
        retval = 0;
        break;
    case 3:
        Uart3_Init(Baud, SET);
        retval = 0;
        break;
    case 4:
        Uart4_Init(Baud, SET);
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
int State_Machine_Bind(UART_mType Channel, D_pFun UART_pFun)
{
    int retval = -1;
#ifdef Exist_UART
    if (UART_pFun == NULL)
    {
        return retval;
    }
    State_Machine_UART_pFun[Channel] = UART_pFun;
#endif
    return retval;
}

int fputc(int ch, FILE *f)      //printf
{
#ifdef DEBUG_OUT
    #ifdef Exist_UART
    Base_UART_Send_Byte(DEBUG_OUT,(uint8_t)ch);
    #endif
#endif // DEBUG
    return (ch);
}

// 你找中断？UART的中断通过函数回调给MODE了！ 
