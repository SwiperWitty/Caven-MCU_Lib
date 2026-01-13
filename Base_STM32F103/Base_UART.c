#include "Base_UART.h" 

#if Exist_UART

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
void Base_UART_Send_Data(UART_mType Channel,uint16_t Data)
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
	USART_SendData(uart_Temp, Data);
}

#if DMA_UART
    #if Exist_UART & OPEN_0010
uint8_t DMA_UART1_Buff[UART_BUFF_MAX];
    #endif
    #if Exist_UART & OPEN_0100
uint8_t DMA_UART2_Buff[UART_BUFF_MAX];
    #endif
    #if Exist_UART & OPEN_1000
uint8_t DMA_UART3_Buff[UART_BUFF_MAX];
    #endif
#endif
void Base_UART_DMA_Send_Data(UART_mType Channel,const uint8_t *Data,int Length)
{
#if DMA_UART
    static char dma_send_First = 0;
    
    DMA_InitTypeDef dma_init_struct;
    DMA_Channel_TypeDef *Temp_DMA_Channel;
    uint32_t DMAy_FLAG;
    uint8_t *p_DMA_BUFF = NULL;
    
    switch (Channel)
    {
        case 0:
        {
    #if Exist_UART & OPEN_0001

    #endif
        }break;
        case 1:
        {
    #if Exist_UART & OPEN_0010
            p_DMA_BUFF = DMA_UART1_Buff;
            uart_Temp = USART1;
            DMAy_FLAG = DMA1_FLAG_TC4;
            Temp_DMA_Channel = DMA1_Channel4;
            if (uart1_enable == 0)
            {
                return;
            }
    #endif
        }break;
        case 2:
        {
    #if Exist_UART & OPEN_0100
            p_DMA_BUFF = DMA_UART2_Buff;
            uart_Temp = USART2;
            DMAy_FLAG = DMA1_FLAG_TC7;
            Temp_DMA_Channel = DMA1_Channel7;
            if (uart2_enable == 0)
            {
                return;
            }
    #endif
        }break;
        case 3:
        {
    #if Exist_UART & OPEN_1000
            p_DMA_BUFF = DMA_UART3_Buff;
            uart_Temp = USART3;
            DMAy_FLAG = DMA1_FLAG_TC2;
            Temp_DMA_Channel = DMA1_Channel2;   // usart3 tx
            if (uart3_enable == 0)
            {
                return;
            }
    #endif
        }break;
        case 4:
        {
    #if Exist_UART & OPEN_10000
            uart_Temp = UART4;
            p_DMA_BUFF = NULL;
            if (uart4_enable == 0)
            {
                return;
            }
    #endif
        }break;
    default:
        return;
    }
    // 开始DMA
    if (Data == NULL || p_DMA_BUFF == NULL || (Length <= 0) || Length > UART_BUFF_MAX) {
        return;
    }
    if ((dma_send_First & (0x01 << Channel)) == 0)            // 当前通道首次DMA，不等
    {
        dma_send_First |= (0x01 << Channel);
		DMA_DeInit(Temp_DMA_Channel);
		DMA_ClearFlag(DMAy_FLAG);
		
		dma_init_struct.DMA_PeripheralBaseAddr = (uint32_t)&uart_Temp->DR;  // 外设地址，如&USART1->DR:cite[1]
		dma_init_struct.DMA_MemoryBaseAddr = (uint32_t)p_DMA_BUFF;      // 内存源地址:cite[1]
		dma_init_struct.DMA_DIR = DMA_DIR_PeripheralDST;    // 发送是方向是外设到内存，外设作为目的地
		dma_init_struct.DMA_BufferSize = Length;         // 传输数据量:cite[1]
		dma_init_struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址固定:cite[1]
		dma_init_struct.DMA_MemoryInc = DMA_MemoryInc_Enable;          // 内存地址自增:cite[1]
		dma_init_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度8位:cite[1]
		dma_init_struct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度8位:cite[1]
		dma_init_struct.DMA_Mode = DMA_Mode_Normal;         // 正常模式（单次传输）:cite[1]/DMA_Mode_Circular
		dma_init_struct.DMA_Priority = DMA_Priority_Medium; // 优先级：中:cite[1]
		dma_init_struct.DMA_M2M = DMA_M2M_Disable;          // 非内存到内存模式:cite[1]
		DMA_Init(Temp_DMA_Channel, &dma_init_struct);
		
		DMA_Cmd(Temp_DMA_Channel, DISABLE);
		DMA_ITConfig(Temp_DMA_Channel, DMA_IT_TC, DISABLE);
    }
	else
	{
		while(DMA_GetFlagStatus(DMAy_FLAG) == 0);    /* Wait USART TX DMA1 Transfer Complete */
		DMA_ClearFlag(DMAy_FLAG);
	}
    
	DMA_Cmd(Temp_DMA_Channel, DISABLE);
	memcpy(p_DMA_BUFF,Data,Length);                     // 一定等上一个发送完成才能修改
	DMA_SetCurrDataCounter(Temp_DMA_Channel, Length);
	USART_DMACmd(uart_Temp, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(Temp_DMA_Channel, ENABLE);    /* usart tx begin dma transmitting */
#endif  
}

#endif

#if (Exist_UART & OPEN_0001)
void Uart0_Init(int Baud,int Set)
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		// 设置中断组，4位抢占优先级，4位响应优先级
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	// AFIO复用功能模块时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    #if UART1_REMAP == OPEN_0000
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; // TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    #elif UART1_REMAP == OPEN_0001
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);		// 串口1重映射
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TXD
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	  //RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    #endif
    /***/
    USART_InitStructure.USART_BaudRate = Baud; // 波特率；
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 数据位8位；
    USART_InitStructure.USART_StopBits = USART_StopBits_1; // 停止位1位；
    USART_InitStructure.USART_Parity = USART_Parity_No ; // 无校验位；
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//
    USART_Init(uart_Temp, &USART_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // 响应优先级
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
    #if UART3_REMAP == OPEN_0000
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //USART3 TX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //USART3 RX；
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    #elif UART3_REMAP == OPEN_0001
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);		// 串口3重映射，与串口4 IO一致（STM32F103RB版本只有串口3没有4）
    
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
void Uart4_Init(int Baud,int Set)
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

int Base_UART_Init(UART_mType Channel, int Baud, int Set)
{
    int retval = -1;
#if DMA_UART
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif
#if Exist_UART
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
#if Exist_UART
    if (UART_pFun == NULL)
    {
        return retval;
    }
    State_Machine_UART_pFun[Channel] = UART_pFun;
#endif
    return retval;
}

int fputc(int ch, FILE *f)      // ptf
{
#if DEBUG_CH
    #if Exist_UART
    Base_UART_Send_Data((UART_mType)DEBUG_CH,(uint8_t)ch);
    #endif
#endif // DEBUG
    return (ch);
}

// 你找中断？UART的中断通过函数回调给MODE了！ 
