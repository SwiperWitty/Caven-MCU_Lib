#include "Base_UART.h"

#ifdef Exist_UART
static usart_type *Temp;

#define RXD_Falg USART_RDBF_FLAG //  接收标志
#define TXD_Falg USART_TDC_FLAG  //  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

static D_pFun State_Machine_UART0_pFun = NULL;
static D_pFun State_Machine_UART1_pFun = NULL;
static D_pFun State_Machine_UART2_pFun = NULL;
static D_pFun State_Machine_UART3_pFun = NULL;
static D_pFun State_Machine_UART4_pFun = NULL;

static char UART_RXD_Flag(UART_mType Channel)
{
    char res;
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
    res = usart_flag_get(Temp, RXD_Falg);
    return res;
}

static void UART_RXD_Flag_Clear(UART_mType Channel)
{
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
    usart_flag_clear(Temp, RXD_Falg);
}

/*  发送 接收    */

// 接收
static uint16_t UART_RXD_Receive(UART_mType Channel) // RXD 读取值
{
    uint16_t res;
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
    res = usart_data_receive(Temp);
    return res;
}

// 发送
void Base_UART_Send_Byte(UART_mType Channel, uint16_t DATA)
{
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

    while (usart_flag_get(Temp, TXD_Falg) == RESET)
        ;
    usart_data_transmit(Temp, DATA);
    // usart_flag_clear(Temp, TXD_Falg);        //可以不要
}

void Base_UART_DMA_Send_Data(UART_mType Channel, const uint8_t *DATA, int Length)
{
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
}

#endif

// 驱动初始化及回调函数
#if (Exist_UART & OPEN_0001)
static void Uart0_Init(int Baud, int SET)
{
}

void UART0_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH0;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART0_pFun != NULL)
        {
            State_Machine_UART0_pFun(&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_0010)
static void Uart1_Init(int Baud, int Set)
{
    confirm_state set = FALSE;
    Temp = USART1;
    usart_reset(Temp);
    if (Set)
        set = TRUE;

    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, set);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_9; // Tx
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_10; // Rx
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOA, &gpio_init_struct);

    nvic_priority_group_config(NVIC_PRIORITY_GROUP_0);
    nvic_irq_enable(USART1_IRQn, 0, 1);

    usart_init(Temp, Baud, USART_DATA_8BITS, USART_STOP_1_BIT); // 波特率、位数、停止位
    usart_transmitter_enable(Temp, TRUE);                       // 发送使能
    usart_receiver_enable(Temp, TRUE);                          // 接收使能

    usart_parity_selection_config(Temp, USART_PARITY_NONE); // 无奇偶校验
    usart_interrupt_enable(Temp, USART_RDBF_INT, TRUE);
    usart_enable(Temp, TRUE);
}

void UART1_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH1;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART1_pFun != NULL)
        {
            State_Machine_UART1_pFun(&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_0100)
void Uart2_Init(int Baud, int Set)
{
    confirm_state set = FALSE;
    Temp = USART2;
    usart_reset(Temp);
    if (Set)
        set = TRUE;

    crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, set);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_2; // Tx
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_3; // Rx
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOA, &gpio_init_struct);

    nvic_priority_group_config(NVIC_PRIORITY_GROUP_0);
    nvic_irq_enable(USART2_IRQn, 0, 2);

    usart_init(Temp, Baud, USART_DATA_8BITS, USART_STOP_1_BIT); // 波特率、位数、停止位
    usart_transmitter_enable(Temp, TRUE);                       // 发送使能
    usart_receiver_enable(Temp, TRUE);                          // 接收使能

    usart_parity_selection_config(Temp, USART_PARITY_NONE); // 无奇偶校验
    usart_interrupt_enable(Temp, USART_RDBF_INT, TRUE);
    usart_enable(Temp, TRUE);
}

void UART2_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH2;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART2_pFun != NULL)
        {
            State_Machine_UART2_pFun(&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_1000)
void Uart3_Init(int Baud, int Set)
{
    confirm_state set = FALSE;
    Temp = USART3;
    usart_reset(Temp);
    if (Set)
        set = TRUE;

    crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, set); // 重映射时钟
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    gpio_pin_remap_config(USART3_GMUX_0001, TRUE); // 重映射串口
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_10; // Tx
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOC, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_11; // Rx
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOC, &gpio_init_struct);

    nvic_priority_group_config(NVIC_PRIORITY_GROUP_0);
    nvic_irq_enable(USART3_IRQn, 0, 3);

    usart_init(Temp, Baud, USART_DATA_8BITS, USART_STOP_1_BIT); // 波特率、位数、停止位
    usart_transmitter_enable(Temp, TRUE);                       // 发送使能
    usart_receiver_enable(Temp, TRUE);                          // 接收使能

    usart_parity_selection_config(Temp, USART_PARITY_NONE); // 无奇偶校验
    usart_interrupt_enable(Temp, USART_RDBF_INT, TRUE);
    usart_enable(Temp, TRUE);
}

void UART3_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH3;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART3_pFun != NULL)
        {
            State_Machine_UART3_pFun(&temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_10000)
void Uart4_Init(int Baud, int Set)
{
    confirm_state set = FALSE;
    Temp = UART4;
    usart_reset(Temp);
    if (Set)
        set = TRUE;

    crm_periph_clock_enable(CRM_UART4_PERIPH_CLOCK, set);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_10; // Tx
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOC, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_11; // Rx
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOC, &gpio_init_struct);

    nvic_priority_group_config(NVIC_PRIORITY_GROUP_0);
    nvic_irq_enable(UART4_IRQn, 0, 4);

    usart_init(Temp, Baud, USART_DATA_8BITS, USART_STOP_1_BIT); // 波特率、位数、停止位
    usart_transmitter_enable(Temp, TRUE);                       // 发送使能
    usart_receiver_enable(Temp, TRUE);                          // 接收使能

    usart_parity_selection_config(Temp, USART_PARITY_NONE); // 无奇偶校验
    usart_interrupt_enable(Temp, USART_RDBF_INT, TRUE);
    usart_enable(Temp, TRUE);
}

void UART4_HANDLERIT()
{
    u8 temp;
    UART_mType UART_CH = m_UART_CH4;
    if (UART_RXD_Flag(UART_CH))
    {
        temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART4_pFun != NULL)
        {
            State_Machine_UART4_pFun(&temp);
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
    switch (Channel)
    {
    case 0:
        State_Machine_UART0_pFun = UART_pFun;
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
    case 4:
        State_Machine_UART4_pFun = UART_pFun;
        retval = 0;
        break;
    default:
        break;
    }
#endif
    return retval;
}

// printf
int fputc(int ch, FILE *f)
{
#ifdef DEBUG_OUT
#ifdef Exist_UART
    Base_UART_Send_Byte(DEBUG_OUT, (uint8_t)ch);
#endif
#endif // DEBUG
    return (ch);
}

// 你找中断？UART的中断通过函数回调给MODE了！
