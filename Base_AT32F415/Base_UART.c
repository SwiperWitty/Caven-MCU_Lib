#include "Base_UART.h" 

#ifdef Exist_UART
static USART_Type * Temp;

#endif

void Uart1_Init(int Baud,int SET)
{
#ifdef UART1_EXIST
    FunctionalState set = DISABLE;
    if (SET)
        set = ENABLE;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);        //IO复用（GPIO->UART）
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1, set);
    GPIO_InitType GPIO_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    USART_InitType USART_InitStructure;

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;            //  RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_9;            //  TXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = set;
    NVIC_Init(&NVIC_InitStructure);

    USART_INTConfig(USART1, USART_INT_RDNE, set);
    USART_Cmd(USART1, set);
#endif
}

void Uart2_Init(int Baud,int SET)
{    
#ifdef UART2_EXIST
    FunctionalState set = DISABLE;
    if (SET)
        set = ENABLE;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2, set);
    GPIO_InitType GPIO_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    USART_InitType USART_InitStructure;

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_3;         //RXD
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = set;
    NVIC_Init(&NVIC_InitStructure);

    USART_INTConfig(USART2, USART_INT_RDNE, set);
    USART_Cmd(USART2, set);
#endif
}

void Uart3_Init(int Baud,int SET)
{
#ifdef UART3_EXIST
    FunctionalState set = DISABLE;
    if (SET)
        set = ENABLE;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);        //IO复用（GPIO->UART）
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART3, set);
    GPIO_PinsRemapConfig(GPIO_PartialRemap_USART3, ENABLE);     //完全重映射串口3
    GPIO_InitType GPIO_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    USART_InitType USART_InitStructure;

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Enable the USART3 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = set;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_INTConfig(USART3, USART_INT_RDNE, set);
    USART_Cmd(USART3, set);
#endif
}

void Uart4_Init(int Baud,int SET)
{
#ifdef UART4_EXIST
    FunctionalState set = DISABLE;
    if (SET)
        set = ENABLE;

    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_UART4, set);
    GPIO_InitType GPIO_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    USART_InitType USART_InitStructure;

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pins = GPIO_Pins_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = Baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    /* Enable the USART4 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelCmd = set;
    NVIC_Init(&NVIC_InitStructure);

    USART_INTConfig(UART4, USART_INT_RDNE, set);
    USART_Cmd(UART4, set);
#endif
}

void Uart5_Init(int Baud,int SET)
{
}

char UART_RXD_Flag(char Channel)
{
    char res;
    switch (Channel)
    {
    case 1:
#ifdef UART1_EXIST
        Temp = USART1;
#endif
        break;
    case 2:
#ifdef UART1_EXIST
        Temp = USART2;
#endif
        break;
    case 3:
#ifdef UART3_EXIST
        Temp = USART3;
#endif
        break;
    case 4:
#ifdef UART4_EXIST
        Temp = USART4;
#endif
        break;
    case 5:
#ifdef UART5_EXIST
        Temp = USART5;
#endif
        break;
    default:
        return (0);
    }
#ifdef Exist_UART
    res = USART_GetITStatus(Temp,RXD_Falg);
#endif
    return res;
}

void UART_RXD_Flag_Clear(char Channel)
{
    switch (Channel)
    {
    case 1:
#ifdef UART1_EXIST
        Temp = USART1;
#endif
        break;
    case 2:
#ifdef UART1_EXIST
        Temp = USART2;
#endif
        break;
    case 3:
#ifdef UART3_EXIST
        Temp = USART3;
#endif
        break;
    case 4:
#ifdef UART4_EXIST
        Temp = USART4;
#endif
        break;
    case 5:
#ifdef UART5_EXIST
        Temp = USART5;
#endif
        break;
    default:
        return;
    }
#ifdef Exist_UART
    USART_ClearFlag(Temp, RXD_Falg);
#endif
    return;
}

/*  发送 接收    */

// 接收
uint16_t UART_RXD_Receive(char Channel)     //RXD 读取值
{
    uint16_t res;
    switch (Channel)
    {
    case 1:
#ifdef UART1_EXIST
        Temp = USART1;
#endif
        break;
    case 2:
#ifdef UART1_EXIST
        Temp = USART2;
#endif
        break;
    case 3:
#ifdef UART3_EXIST
        Temp = USART3;
#endif
        break;
    case 4:
#ifdef UART4_EXIST
        Temp = USART4;
#endif
        break;
    case 5:
#ifdef UART5_EXIST
        Temp = USART5;
#endif
        break;
    default:
        break;
    }
    #ifdef Exist_UART
    res = USART_ReceiveData(Temp);
    #endif
    return res;
    
}

// 发送
void UART_TXD_Send(char Channel,uint16_t DATA)
{
    switch (Channel)
    {
    case 1:
#ifdef UART1_EXIST
        Temp = USART1;
#endif
        break;
//USART1
    case 2:
#ifdef UART2_EXIST
        Temp = USART2;
#endif
        break;
//USART2
    case 3:
#ifdef UART3_EXIST
        Temp = USART3;
#endif
        break;
//USART3
    case 4:
#ifdef UART4_EXIST
        Temp = USART4;
#endif
        break;
//UART4
    case 5:
#ifdef UART5_EXIST
        Temp = USART5;
#endif
        break;
//UART5
    default:
        return;
//error,直接返回
    }
#ifdef Exist_UART
    while (USART_GetFlagStatus(Temp, TXD_Falg) == 0);  
    USART_SendData(Temp, DATA);
    USART_ClearFlag(Temp, TXD_Falg);
#endif
}

int fputc(int ch, FILE *f)      //printf
{
#ifdef DEBUG_OUT
    #ifdef Exist_UART
//    USART_SendData(USART1,(uint8_t)ch);
//    while (!USART_GetFlagStatus(USART1, TXD_Falg));
    UART_TXD_Send(1,(uint8_t)ch);
    #endif
#endif // DEBUG
    return (ch);
}

