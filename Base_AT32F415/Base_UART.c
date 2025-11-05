#include "Base_UART.h"

#ifdef Exist_UART

#define RXD_Falg USART_RDBF_FLAG //  接收标志
#define TXD_Falg USART_TDC_FLAG  //  [USART_FLAG_TXE]这个只是说明，数据被cpu取走,[USART_FLAG_TC]这是完全发送完成

static usart_type *uart_Temp;
static D_pFun State_Machine_UART_pFun[5];

uint8_t uart0_enable = 0;
uint8_t uart1_enable = 0;
uint8_t uart2_enable = 0;
uint8_t uart3_enable = 0;
uint8_t uart4_enable = 0;

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
    res = usart_flag_get(uart_Temp, RXD_Falg);
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
    usart_flag_clear(uart_Temp, RXD_Falg);
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
    res = usart_data_receive(uart_Temp);
    return res;
}

#endif

// 驱动初始化及回调函数
#if (Exist_UART & OPEN_0001)
static void Uart0_Init(int Baud, int Set)
{
    uart0_enable = Set;
}

void UART0_HANDLERIT()
{
    u8 uart_Temp;
    UART_mType UART_CH = m_UART_CH0;
    if (UART_RXD_Flag(UART_CH))
    {
        uart_Temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&uart_Temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_0010)
static void Uart1_Init(int Baud, int Set)
{
    confirm_state set = FALSE;
    uart_Temp = USART1;
    usart_reset(uart_Temp);
    uart1_enable = Set;
    if (Set)
        set = TRUE;
    
    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, set);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
	#if (UART1_REMAP == OPEN_0000)
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_init_struct.gpio_pins = GPIO_Pin_9; // Tx
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_Pin_10; // Rx
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOA, &gpio_init_struct);
	#elif (UART1_REMAP == OPEN_0001)
	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	gpio_pin_remap_config(USART1_GMUX_0001,TRUE);
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    gpio_init_struct.gpio_pins = GPIO_Pin_6; // Tx
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOB, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_Pin_7; // Rx
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOB, &gpio_init_struct);
	#endif
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(USART1_IRQn, 0, 1);                 //

    usart_init(uart_Temp, Baud, USART_DATA_8BITS, USART_STOP_1_BIT);    // 波特率、位数、停止位
    usart_transmitter_enable(uart_Temp, set);                           // 发送使能
    usart_receiver_enable(uart_Temp, set);                              // 接收使能

    usart_parity_selection_config(uart_Temp, USART_PARITY_NONE);        // 无奇偶校验
    usart_interrupt_enable(uart_Temp, USART_RDBF_INT, set);
    
    usart_enable(uart_Temp, set);
}

void UART1_HANDLERIT()
{
    u8 uart_Temp;
    UART_mType UART_CH = m_UART_CH1;
    if (UART_RXD_Flag(UART_CH))
    {
        uart_Temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&uart_Temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_0100)
void Uart2_Init(int Baud, int Set)
{
    confirm_state set = FALSE;
    uart_Temp = USART2;
    usart_reset(uart_Temp);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    uart2_enable = Set;
    if (Set)
        set = TRUE;
    /* add user code begin usart2_init 0 */
    /* add user code end usart2_init 0 */
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, set);
    
    /* add user code begin usart2_init 1 */
    /* add user code end usart2_init 1 */
    /* configure the TX pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_Pin_2;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);
    /* configure the RX pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pins = GPIO_Pin_3;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);
    /* configure param */
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(USART2_IRQn, 0, 2);                                 // 优先级,中断
    
    usart_init(uart_Temp, Baud, USART_DATA_8BITS, USART_STOP_1_BIT);    // 波特率、位数、停止位
    usart_transmitter_enable(uart_Temp, set);                           // 发送使能
    usart_receiver_enable(uart_Temp, set);                              // 接收使能
    usart_parity_selection_config(uart_Temp, USART_PARITY_NONE);
    
    usart_hardware_flow_control_set(uart_Temp, USART_HARDWARE_FLOW_NONE);
    usart_interrupt_enable(uart_Temp, USART_RDBF_INT, set);             // 中断
    /**
     * Users need to configure USART2 interrupt functions according to the actual application.
     * 1. Call the below function to enable the corresponding USART2 interrupt.
     *     --usart_interrupt_enable(...)
     * 2. Add the user's interrupt handler code into the below function in the at32f403a_407_int.c file.
     *     --void USART2_IRQHandler(void)
     */
    usart_enable(uart_Temp, set);
    /* add user code begin usart2_init 2 */
    /* add user code end usart2_init 2 */
}

void UART2_HANDLERIT()
{
    u8 uart_Temp;
    UART_mType UART_CH = m_UART_CH2;
    if (UART_RXD_Flag(UART_CH))
    {
        uart_Temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&uart_Temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_1000)
void Uart3_Init(int Baud, int Set)
{
    confirm_state set = FALSE;
    uart_Temp = USART3;
    usart_reset(uart_Temp);
    uart3_enable = Set;
    if (Set)
        set = TRUE;
	
    crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, set);
	#if (UART3_REMAP == OPEN_0000)
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    /* configure the TX pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_Pin_10;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOB, &gpio_init_struct);
    /* configure the RX pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pins = GPIO_Pin_11;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOB, &gpio_init_struct);
    gpio_pin_remap_config(USART3_GMUX_0001, TRUE);
	#elif (UART3_REMAP == OPEN_0001)
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	gpio_pin_remap_config(USART3_GMUX_0001, TRUE);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    /* configure the TX pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pins = GPIO_Pin_10;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOC, &gpio_init_struct);
    /* configure the RX pin */
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pins = GPIO_Pin_11;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOC, &gpio_init_struct);
	#endif
    /* configure param */
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(USART3_IRQn, 0, 3);                                 // 优先级
    
    usart_init(uart_Temp, Baud, USART_DATA_8BITS, USART_STOP_1_BIT);    // 波特率、位数、停止位
    usart_transmitter_enable(uart_Temp, set);                           // 发送使能
    usart_receiver_enable(uart_Temp, set);                              // 接收使能
    usart_parity_selection_config(uart_Temp, USART_PARITY_NONE);
    
    usart_hardware_flow_control_set(uart_Temp, USART_HARDWARE_FLOW_NONE);
    usart_interrupt_enable(uart_Temp, USART_RDBF_INT, TRUE);

    usart_enable(uart_Temp, set);
}

void UART3_HANDLERIT()
{
    u8 uart_Temp;
    UART_mType UART_CH = m_UART_CH3;
    if (UART_RXD_Flag(UART_CH))
    {
        uart_Temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&uart_Temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

#if (Exist_UART & OPEN_10000)
void Uart4_Init(int Baud, int Set)
{
    confirm_state set = FALSE;
    uart_Temp = UART4;
    usart_reset(uart_Temp);
    uart4_enable = Set;
    if (Set)
        set = TRUE;
    crm_periph_clock_enable(CRM_UART4_PERIPH_CLOCK, set);
    
}

void UART4_HANDLERIT()
{
    u8 uart_Temp;
    UART_mType UART_CH = m_UART_CH4;
    if (UART_RXD_Flag(UART_CH))
    {
        uart_Temp = UART_RXD_Receive(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&uart_Temp);
        }
        UART_RXD_Flag_Clear(UART_CH);
    }
}

#endif

int Base_UART_Init(UART_mType Channel, int Baud, int Set)
{
    int retval = -1;
	
#ifdef Exist_UART
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_DMA2_PERIPH_CLOCK, TRUE);
    switch (Channel)
    {
    case 0:
		
        break;
    case 1:
	#if (Exist_UART & OPEN_0010)
        Uart1_Init(Baud, SET);
        retval = 0;
	#endif
        break;
    case 2:
	#if (Exist_UART & OPEN_0100)
        Uart2_Init(Baud, SET);
        retval = 0;
	#endif
        break;
    case 3:
	#if (Exist_UART & OPEN_1000)
        Uart3_Init(Baud, SET);
        retval = 0;
	#endif
        break;
    case 4:
//        Uart4_Init(Baud, SET);
        retval = 0;
        break;
    default:
        break;
    }
#endif
    return retval;
}

// 发送
void Base_UART_Send_Data(UART_mType Channel, uint16_t Data)
{
#ifdef Exist_UART
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
    while (usart_flag_get(uart_Temp, TXD_Falg) == RESET);
    usart_data_transmit(uart_Temp, Data);
    usart_flag_clear(uart_Temp, TXD_Falg);        //可以不要
#endif
}

#ifdef DMA_UART
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
void Base_UART_DMA_Send_Data(UART_mType Channel, const uint8_t *Data, int Length)
{
#ifdef DMA_UART
    static char dma_send_First = 0;
    
    dma_init_type dma_init_struct;
    dma_channel_type *Temp_DMA_Channel;
    uint32_t DMAy_FLAG;
    uint8_t *p_DMA_BUFF = NULL;
    
    switch (Channel)
    {
        case 0:
        {
    #if Exist_UART & OPEN_0001
            p_DMA_BUFF = DMA_UART0_Buff;
            uart_Temp = USART0;
            DMAy_FLAG = 0;
            Temp_DMA_Channel = 0;
            if (uart0_enable == 0)
            {
                return;
            }
        return;
    #endif
        }break;
        case 1:
        {
    #if Exist_UART & OPEN_0010
            p_DMA_BUFF = DMA_UART1_Buff;
            uart_Temp = USART1;
            DMAy_FLAG = DMA1_FDT4_FLAG;
            Temp_DMA_Channel = DMA1_CHANNEL4;
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
            DMAy_FLAG = DMA1_FDT7_FLAG;
            Temp_DMA_Channel = DMA1_CHANNEL7;
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
            DMAy_FLAG = DMA1_FDT2_FLAG;
            Temp_DMA_Channel = DMA1_CHANNEL2;   // usart3 tx
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
    if (Data == NULL || p_DMA_BUFF == NULL || (Length <= 0)) {
        return;
    }
    dma_default_para_init(&dma_init_struct);
    if ((dma_send_First & (0x01 << Channel)) == 0)            // 当前通道首次DMA，不等
    {
        dma_send_First |= (0x01 << Channel);
        dma_flag_clear(DMAy_FLAG);
		
		dma_init_struct.buffer_size = 0;                           //
		dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
		dma_init_struct.memory_base_addr = (uint32_t)p_DMA_BUFF;        //
		dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE; 
		dma_init_struct.memory_inc_enable = TRUE;
		dma_init_struct.peripheral_base_addr = (uint32_t)&uart_Temp->dt;    //
		dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
		dma_init_struct.peripheral_inc_enable = FALSE;
		dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
		dma_init_struct.loop_mode_enable = FALSE;
		dma_init(Temp_DMA_Channel, &dma_init_struct);
    }
    else
    {
        while(dma_flag_get(DMAy_FLAG) == RESET);    /* Wait until USART TX DMA1 Transfer Complete */
        dma_flag_clear(DMAy_FLAG);
    }
	dma_channel_enable(Temp_DMA_Channel, FALSE);
    memcpy(p_DMA_BUFF,Data,Length);                     // 一定等上一个发送完成才能修改
    
	dma_data_number_set(Temp_DMA_Channel, Length);
    /* config flexible dma for usartx tx */
    usart_dma_transmitter_enable(uart_Temp, TRUE); 
//    dma_flexible_config(DMA_Temp, Temp_FLEX_Channel, DMA_FLEXIBLE_Temp);
    dma_channel_enable(Temp_DMA_Channel, TRUE);    /* usart tx begin dma transmitting */
#endif
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
    retval = 0;
#endif
    return retval;
}

// printf
int fputc(int ch, FILE *f)
{
#ifdef DEBUG_OUT
	#ifdef Exist_UART
    Base_UART_Send_Data((UART_mType)DEBUG_OUT, (uint8_t)ch);
	#endif
#endif // DEBUG
    return (ch);
}

// 你找中断？UART的中断通过函数回调给MODE了！
