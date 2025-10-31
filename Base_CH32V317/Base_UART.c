#include "Base_UART.h"



#ifdef Exist_UART

#define RXD_Falg    USART_IT_RXNE       //  接收标志
#define TXD_Falg    USART_FLAG_TC       //  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

static D_pFun State_Machine_UART_pFun[5]; //[0-4]

uint8_t uart0_enable = 0;
uint8_t uart1_enable = 0;
uint8_t uart2_enable = 0;
uint8_t uart3_enable = 0;
uint8_t uart4_enable = 0;

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
    case 4:
        Temp = UART4;
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
    case 4:
        Temp = UART4;
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
    case 4:
        Temp = UART4;
        break;
    default:
        return 0;
    }
    res = USART_ReceiveData(Temp);
    return res;
}

void Base_UART_Send_Data(UART_mType Channel,uint16_t Data)
{
    USART_TypeDef * uart_Temp;
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
    USART_SendData(uart_Temp, Data);
}

    #ifdef DMA_UART
uint8_t DMA_UART1_Buff[500];
uint8_t DMA_UART2_Buff[500];
uint8_t DMA_UART3_Buff[500];
/*
 *
 */
void Base_UART_DMA_Send_Data(UART_mType Channel,const uint8_t *Data,int Length)
{
    USART_TypeDef * Temp_USART;
    DMA_InitTypeDef DMA_InitStructure = {0};
    DMA_Channel_TypeDef *Temp_DMA_Channel = NULL;
    uint32_t DMAy_FLAG;

    uint8_t *p_DMA_BUFF = NULL;
    static char dma_send_First = 0;

    switch (Channel)
    {
        case 0:
        {
            if (uart0_enable == 0)
            {
                return;
            }
        }
        return;
        case 1:
        {
            if (uart1_enable == 0)
            {
                return;
            }
            Temp_USART = USART1;
            DMAy_FLAG = DMA1_FLAG_TC4;
            p_DMA_BUFF = DMA_UART1_Buff;
            Temp_DMA_Channel = DMA1_Channel4;
        }
        break;
        case 2:
        {
            if (uart2_enable == 0)
            {
                return;
            }
            Temp_USART = USART2;
            DMAy_FLAG = DMA1_FLAG_TC7;
            p_DMA_BUFF = DMA_UART2_Buff;
            Temp_DMA_Channel = DMA1_Channel7;
        }
        break;
        case 3:
        {
            if (uart3_enable == 0)
            {
                return;
            }
            Temp_USART = USART3;
            DMAy_FLAG = DMA1_FLAG_TC2;
            p_DMA_BUFF = DMA_UART3_Buff;
            Temp_DMA_Channel = DMA1_Channel2;
        }
        break;
        case 4:
        {
            if (uart4_enable == 0)
            {
                return;
            }
            Temp_USART = UART4;     //UART - DMA2-5_TX
            p_DMA_BUFF = NULL;
        }
        break;
    default:
        return;
    }

    // 开始DMA
    if (Data == NULL || p_DMA_BUFF == NULL || (Length < 0)) {
        return;
    }

    if ((dma_send_First & (0x01 << Channel)) == 0)            // 当前通道首次DMA，不等
    {
        dma_send_First |= (0x01 << Channel);
        DMA_ClearFlag(DMAy_FLAG);
    }
    else
    {
        while(DMA_GetFlagStatus(DMAy_FLAG) == RESET);   /* Wait until USART TX DMA1 Transfer Complete */
        DMA_ClearFlag(DMAy_FLAG);
    }
    memcpy(p_DMA_BUFF,Data,Length);                     // 一定等上一个发送完成才能修改

    DMA_DeInit(Temp_DMA_Channel);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&Temp_USART->DATAR);   /* USARTx->DATAR: */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)p_DMA_BUFF;             //
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                  // DMA_DIR_PeripheralSRC(RX)
    DMA_InitStructure.DMA_BufferSize = Length;                          //
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(Temp_DMA_Channel, &DMA_InitStructure);

    DMA_Cmd(Temp_DMA_Channel, ENABLE);

    USART_DMACmd(Temp_USART,USART_DMAReq_Tx, ENABLE);
}
    #endif

#endif



// 以下函数很重要，包括功能启动，功能中断处理

#if (Exist_UART & OPEN_0001)
void Uart0_Init(int Baud,int Set)
{

}
#endif

#if (Exist_UART & OPEN_0010)
void Uart1_Init(int Baud,int Set)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
	USART_InitTypeDef USART_InitStructure = {0};
	NVIC_InitTypeDef NVIC_InitStructure = {0};
	USART_TypeDef * Temp_USART = USART1;
	FunctionalState temp;
	uart1_enable = SET;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, temp);	  // USART1  (APB2)
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // AFIO复用功能模块时钟(暂不需要)
//  GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);     //端口复用
    USART_DeInit(Temp_USART);

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
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;					//
    USART_InitStructure.USART_Parity = USART_Parity_No;								//
    USART_InitStructure.USART_StopBits = USART_StopBits_1;							//
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//
    USART_Init(Temp_USART, &USART_InitStructure);
    USART_ITConfig(Temp_USART, RXD_Falg, temp);                                 //

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = temp;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(Temp_USART, temp);
}

void UART1_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH1;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL) {
            State_Machine_UART_pFun[UART_CH](&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_0100)
void Uart2_Init(int Baud,int Set)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    USART_TypeDef * Temp_USART = USART2;
    FunctionalState temp;
    uart2_enable = SET;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, temp);    // USART2  (APB1)
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    // AFIO复用功能模块时钟(暂不需要)
//  GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);         //端口复用
    USART_DeInit(Temp_USART);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;       // RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;       // TXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;         //
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      //
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //
    USART_Init(Temp_USART, &USART_InitStructure);
    USART_ITConfig(Temp_USART, RXD_Falg, temp);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;           // USART2
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;          // 响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = temp;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(Temp_USART, temp);
}

void UART2_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH2;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL) {
            State_Machine_UART_pFun[UART_CH](&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_1000)
void Uart3_Init(int Baud,int Set)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    USART_TypeDef * Temp_USART = USART3;
    FunctionalState temp;
    uart3_enable = SET;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, temp);    // USART3  (APB1)
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    // AFIO复用功能模块时钟(暂不需要)
//  GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);         // 端口复用
    USART_DeInit(Temp_USART);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;      // RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;      // TXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;         //
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      //
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //
    USART_Init(Temp_USART, &USART_InitStructure);
    USART_ITConfig(Temp_USART, RXD_Falg, temp);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;           // USART2
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;          // 响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = temp;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_Cmd(Temp_USART, temp);
}

void UART3_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH3;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL) {
            State_Machine_UART_pFun[UART_CH](&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_10000)
void Uart4_Init(int Baud,int Set)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    USART_TypeDef * Temp_USART = UART4;
    FunctionalState temp;
    uart4_enable = SET;
    if(SET)
        temp = ENABLE;
    else
        temp = DISABLE;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, temp);     // UART4  (APB1)

    USART_DeInit(Temp_USART);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;       // RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;       // TXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;         //
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      //
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //
    USART_Init(Temp_USART, &USART_InitStructure);
    USART_ITConfig(Temp_USART, RXD_Falg, temp);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;            // UART4
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;          // 响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = temp;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(Temp_USART, temp);
}

void UART4_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH4;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL) {
            State_Machine_UART_pFun[UART_CH](&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif


int Base_UART_Init(UART_mType Channel,int Baud,int Set)
{
    int retval = -1;
#ifdef Exist_UART
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
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
    case 4:
        Uart4_Init(Baud,SET);
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
int Base_UART_Once_Flag = 0;
int State_Machine_Bind (UART_mType Channel,D_pFun UART_pFun)
{
    int retval = -1;
#ifdef Exist_UART
    if (Base_UART_Once_Flag == 0) {
        memset(State_Machine_UART_pFun,0,sizeof(State_Machine_UART_pFun));
        Base_UART_Once_Flag = 1;
    }
    if (UART_pFun == NULL)
    {
        return retval;
    }
    State_Machine_UART_pFun[Channel] = UART_pFun;

#endif
    return retval;
}
