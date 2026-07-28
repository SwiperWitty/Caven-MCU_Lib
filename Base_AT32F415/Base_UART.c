#include "Base_UART.h"

#include "at32f415_crm.h"
#include "at32f415_dma.h"
#include "at32f415_gpio.h"
#include "at32f415_misc.h"
#include "at32f415_usart.h"

extern void SYS_Base_Delay(int time, int speed);

#ifndef BUFF_MAX
#define BUFF_MAX UART_BUFF_MAX
#endif

#if DMA_UART1_RX & DMA_UART
static Caven_DoubleBufType Caven_Double_U1;
#endif
#if DMA_UART2_RX & DMA_UART
static Caven_DoubleBufType Caven_Double_U2;
#endif
#if DMA_UART3_RX & DMA_UART
static Caven_DoubleBufType Caven_Double_U3;
#endif
#if DMA_UART4_RX & DMA_UART
static Caven_DoubleBufType Caven_Double_U4;
#endif

static iD_pFun State_Machine_UART_pFun[6];

static uint8_t uart0_enable = 0;
static uint8_t uart1_enable = 0;
static uint8_t uart2_enable = 0;
static uint8_t uart3_enable = 0;
static uint8_t uart4_enable = 0;

static uint8_t uart0_dma_enable = 0;
static uint8_t uart1_dma_enable = 0;
static uint8_t uart2_dma_enable = 0;
static uint8_t uart3_dma_enable = 0;
static uint8_t uart4_dma_enable = 0;

#if (Exist_UART & OPEN_0010) && DMA_UART
static uint8_t DMA_UART1_Buff[UART_BUFF_MAX];
#endif
#if (Exist_UART & OPEN_0100) && DMA_UART
static uint8_t DMA_UART2_Buff[UART_BUFF_MAX];
#endif
#if (Exist_UART & OPEN_1000) && DMA_UART
static uint8_t DMA_UART3_Buff[UART_BUFF_MAX];
#endif
#if (Exist_UART & OPEN_10000) && DMA_UART
static uint8_t DMA_UART4_Buff[UART_BUFF_MAX];
#endif

static int UART_Wait_TX_Flag(UART_mType Channel)
{
#if Exist_UART
    usart_type *uart_Temp = NULL;
    uint32_t retry = 0;

    switch (Channel)
    {
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

    do
    {
        if (usart_flag_get(uart_Temp, USART_TDC_FLAG) != RESET)
        {
            return 1;
        }
        SYS_Base_Delay(1, 1000);
    } while (++retry < UART_WAIT_MAX);
#endif
    return 0;
}

int UART_Wait_DMA_Flag(uint32_t Flag)
{
#if Exist_UART
    uint32_t retry = 0;

    if (Flag == 0)
    {
        return 1;
    }

    do
    {
        if (dma_flag_get(Flag) != RESET)
        {
            return 1;
        }
        SYS_Base_Delay(100, 1000);
    } while (++retry < UART_WAIT_MAX);
#endif
    return 0;
}

static int UART_RXD_Flag(UART_mType Channel)
{
#if Exist_UART
    usart_type *uart_Temp = NULL;

    switch (Channel)
    {
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

    return (usart_flag_get(uart_Temp, USART_RDBF_FLAG) != RESET) ? 1 : 0;
#else
    (void)Channel;
    return 0;
#endif
}

static uint16_t UART_RXD_Receive(UART_mType Channel)
{
#if Exist_UART
    switch (Channel)
    {
    case 1:
        return usart_data_receive(USART1);
    case 2:
        return usart_data_receive(USART2);
    case 3:
        return usart_data_receive(USART3);
    case 4:
        return usart_data_receive(UART4);
    default:
        return 0;
    }
#else
    (void)Channel;
    return 0;
#endif
}

static void UART_RXD_Flag_Clear(UART_mType Channel)
{
#if Exist_UART
    usart_type *uart_Temp = NULL;

    switch (Channel)
    {
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

    usart_flag_clear(uart_Temp, USART_RDBF_FLAG);
#else
    (void)Channel;
#endif
}

static void DMA_RX_IDLE_Clear(UART_mType Channel)
{
#if Exist_UART
    usart_type *uart_Temp = NULL;

    switch (Channel)
    {
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

    usart_flag_clear(uart_Temp, USART_IDLEF_FLAG);
#else
    (void)Channel;
#endif
}

void Base_UART_Send_Data(UART_mType Channel, uint16_t Data)
{
#if Exist_UART
    usart_type *uart_Temp = NULL;

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

    usart_flag_clear(uart_Temp, USART_TDC_FLAG);
    usart_data_transmit(uart_Temp, Data);
    if (UART_Wait_TX_Flag(Channel) == 0)
    {
        return;
    }
#endif
}

void Base_UART_DMA_Send_Data(UART_mType Channel, const uint8_t *Data, int Length)
{
#if Exist_UART
    usart_type *Temp_USART = NULL;
    dma_init_type DMA_InitStructure = {0};
    dma_channel_type *Temp_DMA_Channel = NULL;
    uint32_t DMAy_FLAG = 0;
    uint8_t *p_DMA_BUFF = NULL;
    static uint8_t dma_send_First = 0;

    if (Data == NULL || Length <= 0)
    {
        return;
    }

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
#if (Exist_UART & OPEN_0010) && DMA_UART
        if ((uart1_dma_enable & UART_DMA_TX_ENABLE) == UART_DMA_TX_ENABLE)
        {
            p_DMA_BUFF = DMA_UART1_Buff;
            Temp_USART = USART1;
            DMAy_FLAG = DMA1_FDT5_FLAG;
            Temp_DMA_Channel = DMA1_CHANNEL5;
        }
#endif
        break;
    case 2:
        if (uart2_enable == 0)
        {
            return;
        }
#if (Exist_UART & OPEN_0100) && DMA_UART
        if ((uart2_dma_enable & UART_DMA_TX_ENABLE) == UART_DMA_TX_ENABLE)
        {
            p_DMA_BUFF = DMA_UART2_Buff;
            Temp_USART = USART2;
            DMAy_FLAG = DMA1_FDT7_FLAG;
            Temp_DMA_Channel = DMA1_CHANNEL7;
        }
#endif
        break;
    case 3:
        if (uart3_enable == 0)
        {
            return;
        }
#if (Exist_UART & OPEN_1000) && DMA_UART
        if ((uart3_dma_enable & UART_DMA_TX_ENABLE) == UART_DMA_TX_ENABLE)
        {
            p_DMA_BUFF = DMA_UART3_Buff;
            Temp_USART = USART3;
            DMAy_FLAG = DMA1_FDT2_FLAG;
            Temp_DMA_Channel = DMA1_CHANNEL2;
        }
#endif
        break;
    case 4:
        if (uart4_enable == 0)
        {
            return;
        }
        break;
    default:
        return;
    }

    if (Temp_USART == NULL || Temp_DMA_Channel == NULL || p_DMA_BUFF == NULL)
    {
        while (Length-- > 0)
        {
            Base_UART_Send_Data(Channel, *Data++);
        }
        return;
    }

    if (Length > UART_BUFF_MAX)
    {
        Length = UART_BUFF_MAX;
    }

    if ((dma_send_First & (0x01U << Channel)) == 0)
    {
        dma_send_First |= (uint8_t)(0x01U << Channel);
        if (DMAy_FLAG != 0)
        {
            dma_flag_clear(DMAy_FLAG);
        }
        dma_reset(Temp_DMA_Channel);
        dma_default_para_init(&DMA_InitStructure);
        DMA_InitStructure.peripheral_base_addr = (uint32_t)&Temp_USART->dt;
        DMA_InitStructure.memory_base_addr = (uint32_t)p_DMA_BUFF;
        DMA_InitStructure.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
        DMA_InitStructure.buffer_size = 0;
        DMA_InitStructure.peripheral_inc_enable = FALSE;
        DMA_InitStructure.memory_inc_enable = TRUE;
        DMA_InitStructure.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
        DMA_InitStructure.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
        DMA_InitStructure.loop_mode_enable = FALSE;
        DMA_InitStructure.priority = DMA_PRIORITY_VERY_HIGH;
        dma_init(Temp_DMA_Channel, &DMA_InitStructure);

        if (Channel == 1)
        {
            dma_flexible_config(DMA1, FLEX_CHANNEL5, DMA_FLEXIBLE_UART1_TX);
        }
        else if (Channel == 2)
        {
            dma_flexible_config(DMA1, FLEX_CHANNEL7, DMA_FLEXIBLE_UART2_TX);
        }
        else if (Channel == 3)
        {
            dma_flexible_config(DMA1, FLEX_CHANNEL2, DMA_FLEXIBLE_UART3_TX);
        }
        dma_interrupt_enable(Temp_DMA_Channel, DMA_FDT_INT, TRUE);
    }
    else
    {
        if (UART_Wait_DMA_Flag(DMAy_FLAG) == 0)
        {
            return;
        }
        if (DMAy_FLAG != 0)
        {
            dma_flag_clear(DMAy_FLAG);
        }
    }

    dma_channel_enable(Temp_DMA_Channel, FALSE);
    memcpy(p_DMA_BUFF, Data, (size_t)Length);
    dma_data_number_set(Temp_DMA_Channel, (uint16_t)Length);
    usart_dma_transmitter_enable(Temp_USART, TRUE);
    dma_channel_enable(Temp_DMA_Channel, TRUE);
#endif
}

void Base_UART_DMA_Send_Buff(UART_mType Channel, const uint8_t *Data, int Length)
{
#if Exist_UART
    uint8_t *temp_buff = (uint8_t *)Data;
    int temp_num = 0, temp_run = Length;

    if (Data == NULL || Length <= 0)
    {
        return;
    }
    while (temp_run > 0)
    {
        temp_num = temp_run;
        if (temp_num > UART_BUFF_MAX)
        {
            temp_num = UART_BUFF_MAX;
        }
        Base_UART_DMA_Send_Data(Channel, temp_buff, temp_num);
        temp_buff += temp_num;
        temp_run -= temp_num;
    }
#endif
}

static void Uart_DMA_RX_Switch(UART_mType Channel, Caven_DoubleBufType *cache, uint8_t full)
{
#if DMA_UART
    usart_type *Temp_USART = NULL;
    dma_init_type DMA_InitStructure = {0};
    dma_channel_type *Temp_DMA_Channel = NULL;
    uint8_t *p_DMA_BUFF = NULL;
    uint8_t temp_val = 0;
    int temp_num = 0;
    int temp_key = 0;

    if (cache == NULL)
    {
        return;
    }

    switch (Channel)
    {
    case 1:
        Temp_USART = USART1;
        Temp_DMA_Channel = DMA1_CHANNEL5;
        temp_key = uart1_dma_enable & UART_DMA_RX_ENABLE;
        break;
    case 2:
        Temp_USART = USART2;
        Temp_DMA_Channel = DMA1_CHANNEL6;
        temp_key = uart2_dma_enable & UART_DMA_RX_ENABLE;
        break;
    case 3:
        Temp_USART = USART3;
        Temp_DMA_Channel = DMA1_CHANNEL3;
        temp_key = uart3_dma_enable & UART_DMA_RX_ENABLE;
        break;
    case 4:
        Temp_USART = UART4;
        temp_key = uart4_dma_enable & UART_DMA_RX_ENABLE;
        break;
    default:
        return;
    }

    if (Temp_USART == NULL || Temp_DMA_Channel == NULL)
    {
        return;
    }

    if (cache->buf_flag[cache->r_buf_idx] > 0 && full == 0)
    {
        cache->w_buf_event = 1;
        return;
    }

    if (temp_key)
    {
        dma_channel_enable(Temp_DMA_Channel, FALSE);
        temp_num = BUFF_MAX - dma_data_number_get(Temp_DMA_Channel);
    }
    else
    {
        cache->buf_flag[cache->r_buf_idx] = 0;
        temp_num = 0;
    }

    if (cache->buf_flag[cache->r_buf_idx] > 0 && full > 0)
    {
        cache->buf_flag[cache->w_buf_idx] = 0;
        cache->buf_len[cache->w_buf_idx] = 0;
        cache->w_buf_event = 0;
    }
    else if (temp_num > 0)
    {
        cache->buf_len[cache->w_buf_idx] = (uint16_t)temp_num;
        temp_val = cache->r_buf_idx;
        cache->r_buf_idx = cache->w_buf_idx;
        cache->w_buf_idx = temp_val;
        cache->w_buf_event = 0;
        cache->buf_len[cache->w_buf_idx] = 0;
        cache->buf_flag[cache->w_buf_idx] = 0;
        cache->r_buf_run = 0;
        cache->buf_flag[cache->r_buf_idx] = 1;
    }
    else
    {
        cache->r_buf_idx = 1;
        cache->w_buf_idx = 0;
        cache->r_buf_run = 0;
        cache->buf_flag[cache->r_buf_idx] = 0;
        cache->buf_flag[cache->w_buf_idx] = 0;
        cache->buf_len[cache->r_buf_idx] = 0;
        cache->buf_len[cache->w_buf_idx] = 0;
        cache->w_buf_event = 0;
    }

    p_DMA_BUFF = cache->buf[cache->w_buf_idx];

    dma_reset(Temp_DMA_Channel);
    dma_default_para_init(&DMA_InitStructure);
    DMA_InitStructure.peripheral_base_addr = (uint32_t)&Temp_USART->dt;
    DMA_InitStructure.memory_base_addr = (uint32_t)p_DMA_BUFF;
    DMA_InitStructure.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
    DMA_InitStructure.buffer_size = BUFF_MAX;
    DMA_InitStructure.peripheral_inc_enable = FALSE;
    DMA_InitStructure.memory_inc_enable = TRUE;
    DMA_InitStructure.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    DMA_InitStructure.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    DMA_InitStructure.loop_mode_enable = FALSE;
    DMA_InitStructure.priority = DMA_PRIORITY_VERY_HIGH;
    dma_init(Temp_DMA_Channel, &DMA_InitStructure);

    if (Channel == 1)
    {
        dma_flexible_config(DMA1, FLEX_CHANNEL5, DMA_FLEXIBLE_UART1_RX);
    }
    else if (Channel == 2)
    {
        dma_flexible_config(DMA1, FLEX_CHANNEL6, DMA_FLEXIBLE_UART2_RX);
    }
    else if (Channel == 3)
    {
        dma_flexible_config(DMA1, FLEX_CHANNEL3, DMA_FLEXIBLE_UART3_RX);
    }
    dma_interrupt_enable(Temp_DMA_Channel, DMA_FDT_INT, TRUE);
    usart_dma_receiver_enable(Temp_USART, TRUE);
    dma_channel_enable(Temp_DMA_Channel, TRUE);
#else
    (void)Channel;
    (void)cache;
    (void)full;
#endif
}

#if (Exist_UART & OPEN_0010)
void Uart1_Init(int Baud, int Set)
{
    gpio_init_type GPIO_InitStructure = {0};
    usart_type *Temp_USART = USART1;
    confirm_state temp;

    uart1_enable = 0;
    uart1_dma_enable = 0;
    temp = (Set != 0) ? TRUE : FALSE;

    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, temp);
    usart_reset(Temp_USART);

#if (UART1_REMAP == OPEN_0000)
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_default_para_init(&GPIO_InitStructure);
    GPIO_InitStructure.gpio_pins = GPIO_PINS_10;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.gpio_pins = GPIO_PINS_9;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_MUX;
    GPIO_InitStructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    GPIO_InitStructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(GPIOA, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.gpio_pins = GPIO_PINS_9 | GPIO_PINS_10;
        GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
        gpio_init(GPIOA, &GPIO_InitStructure);
    }
#elif (UART1_REMAP == OPEN_0001)
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
    gpio_pin_remap_config(USART1_MUX, TRUE);
    crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
    gpio_default_para_init(&GPIO_InitStructure);
    GPIO_InitStructure.gpio_pins = GPIO_PINS_7;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.gpio_pins = GPIO_PINS_6;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_MUX;
    GPIO_InitStructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    GPIO_InitStructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(GPIOB, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.gpio_pins = GPIO_PINS_6 | GPIO_PINS_7;
        GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
        gpio_init(GPIOB, &GPIO_InitStructure);
    }
#endif
    usart_init(Temp_USART, Baud, USART_DATA_8BITS, USART_STOP_1_BIT);	// 波特率、位数、停止位
    usart_transmitter_enable(Temp_USART, temp);							// 发送使能
    usart_receiver_enable(Temp_USART, temp);							// 接收使能
    usart_parity_selection_config(Temp_USART, USART_PARITY_NONE);		// 无奇偶校验
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
#if DMA_UART1_RX & DMA_UART
    if (temp)
    {
        Uart_DMA_RX_Switch(1, &Caven_Double_U1, 0);
        uart1_dma_enable |= UART_DMA_RX_ENABLE;
    }
	usart_interrupt_enable(Temp_USART, USART_IDLE_INT, temp);
	//
#else
    usart_interrupt_enable(Temp_USART, USART_RDBF_INT, temp);
#endif
#if DMA_UART
    uart1_dma_enable |= UART_DMA_TX_ENABLE;
#endif
    if (Set == 0)
    {
        uart1_dma_enable = 0;
    }
    nvic_irq_enable(USART1_IRQn, 0, 1);
	
    usart_enable(Temp_USART, temp);
    uart1_enable = (uint8_t)Set;
}

void UART1_HANDLERIT ()
{
    uint8_t uart_Temp;
    UART_mType UART_CH = m_UART_CH1;
    usart_type *Temp_USART = USART1;

    if (UART_RXD_Flag(UART_CH))
    {
        uart_Temp = (uint8_t)UART_RXD_Receive(UART_CH);
        UART_RXD_Flag_Clear(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&uart_Temp);
        }
    }
    if (usart_flag_get(Temp_USART, USART_IDLEF_FLAG) != RESET)
    {
        uart_Temp = (uint8_t)UART_RXD_Receive(UART_CH);
        DMA_RX_IDLE_Clear(UART_CH);
#if DMA_UART1_RX & DMA_UART
        Uart_DMA_RX_Switch(UART_CH, &Caven_Double_U1, 0);
#endif
    }
}
#endif

#if (Exist_UART & OPEN_0100)
void Uart2_Init(int Baud, int Set)
{
    gpio_init_type GPIO_InitStructure = {0};
    usart_type *Temp_USART = USART2;
    confirm_state temp;

    uart2_enable = 0;
    uart2_dma_enable = 0;
    temp = (Set != 0) ? TRUE : FALSE;

    crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, temp);
    usart_reset(Temp_USART);

    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_default_para_init(&GPIO_InitStructure);
    GPIO_InitStructure.gpio_pins = GPIO_PINS_3;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.gpio_pins = GPIO_PINS_2;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_MUX;
    GPIO_InitStructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    GPIO_InitStructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(GPIOA, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.gpio_pins = GPIO_PINS_2 | GPIO_PINS_3;
        GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
        gpio_init(GPIOA, &GPIO_InitStructure);
    }

    usart_init(Temp_USART, Baud, USART_DATA_8BITS, USART_STOP_1_BIT);	// 波特率、位数、停止位
    usart_transmitter_enable(Temp_USART, temp);							// 发送使能
    usart_receiver_enable(Temp_USART, temp);							// 接收使能
    usart_parity_selection_config(Temp_USART, USART_PARITY_NONE);		// 无奇偶校验
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
#if DMA_UART2_RX & DMA_UART
    if (temp)
    {
        Uart_DMA_RX_Switch(m_UART_CH2, &Caven_Double_U2, 0);
        uart2_dma_enable |= UART_DMA_RX_ENABLE;
    }
	usart_interrupt_enable(Temp_USART, USART_IDLE_INT, temp);
	nvic_irq_enable(DMA1_Channel6_IRQn, 1, 2);
#else
    usart_interrupt_enable(Temp_USART, USART_RDBF_INT, temp);
#endif
#if DMA_UART
    uart2_dma_enable |= UART_DMA_TX_ENABLE;
#endif
    if (Set == 0)
    {
        uart2_dma_enable = 0;
    }
    nvic_irq_enable(USART2_IRQn, 0, 2);
	
    usart_enable(Temp_USART, temp);
    uart2_enable = (uint8_t)Set;
}

void UART2_HANDLERIT ()
{
    uint8_t uart_Temp;
    UART_mType UART_CH = m_UART_CH2;
    usart_type *Temp_USART = USART2;

    if (UART_RXD_Flag(UART_CH))
    {
        uart_Temp = (uint8_t)UART_RXD_Receive(UART_CH);
        UART_RXD_Flag_Clear(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&uart_Temp);
        }
    }
    if (usart_flag_get(Temp_USART, USART_IDLEF_FLAG) != RESET)
    {
        uart_Temp = (uint8_t)UART_RXD_Receive(UART_CH);
        (void)uart_Temp;
        DMA_RX_IDLE_Clear(UART_CH);
	#if DMA_UART2_RX & DMA_UART
        Uart_DMA_RX_Switch(UART_CH, &Caven_Double_U2, 0);
	#endif
    }
}

	#if DMA_UART2_RX & DMA_UART
void DMA1_CH6_HANDLERIT ()
{
    UART_mType UART_CH = m_UART_CH2;
    if (dma_flag_get(DMA1_FDT6_FLAG) != RESET)
    {
        dma_flag_clear(DMA1_FDT6_FLAG);
        Uart_DMA_RX_Switch(UART_CH, &Caven_Double_U2, 1);
    }
}
	#endif
#endif

#if (Exist_UART & OPEN_1000)
void Uart3_Init(int Baud, int Set)
{
    gpio_init_type GPIO_InitStructure = {0};
    usart_type *Temp_USART = USART3;
    confirm_state temp;

    uart3_enable = 0;
    uart3_dma_enable = 0;
    temp = (Set != 0) ? TRUE : FALSE;

    crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, temp);
    usart_reset(Temp_USART);

#if (UART3_REMAP == OPEN_0000)
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    gpio_default_para_init(&GPIO_InitStructure);
    GPIO_InitStructure.gpio_pins = GPIO_PINS_11;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.gpio_pins = GPIO_PINS_10;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_MUX;
    GPIO_InitStructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    GPIO_InitStructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(GPIOC, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.gpio_pins = GPIO_PINS_10 | GPIO_PINS_11;
        GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
        gpio_init(GPIOC, &GPIO_InitStructure);
    }
#elif (UART3_REMAP == OPEN_0001)
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
    gpio_pin_remap_config(USART3_MUX_10, TRUE);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_default_para_init(&GPIO_InitStructure);
    GPIO_InitStructure.gpio_pins = GPIO_PINS_6;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.gpio_pins = GPIO_PINS_7;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_MUX;
    GPIO_InitStructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    GPIO_InitStructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(GPIOA, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.gpio_pins = GPIO_PINS_6 | GPIO_PINS_7;
        GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
        gpio_init(GPIOA, &GPIO_InitStructure);
    }
#endif

    usart_init(Temp_USART, Baud, USART_DATA_8BITS, USART_STOP_1_BIT);	// 波特率、位数、停止位
    usart_transmitter_enable(Temp_USART, temp);							// 发送使能
    usart_receiver_enable(Temp_USART, temp);							// 接收使能
    usart_parity_selection_config(Temp_USART, USART_PARITY_NONE);		// 无奇偶校验
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
#if DMA_UART3_RX & DMA_UART
    if (temp)
    {
        Uart_DMA_RX_Switch(m_UART_CH3, &Caven_Double_U3, 0);
        uart3_dma_enable |= UART_DMA_RX_ENABLE;
    }
	usart_interrupt_enable(Temp_USART, USART_IDLE_INT, temp);
	nvic_irq_enable(DMA1_Channel3_IRQn, 1, 3);
#else
    usart_interrupt_enable(Temp_USART, USART_RDBF_INT, temp);
#endif
#if DMA_UART
    uart3_dma_enable |= UART_DMA_TX_ENABLE;
#endif
    if (Set == 0)
    {
        uart3_dma_enable = 0;
    }
    nvic_irq_enable(USART3_IRQn, 0, 3);
	
    usart_enable(Temp_USART, temp);
    uart3_enable = (uint8_t)Set;
}

void UART3_HANDLERIT ()
{
    uint8_t uart_Temp;
    UART_mType UART_CH = m_UART_CH3;
    usart_type *Temp_USART = USART3;

    if (UART_RXD_Flag(UART_CH))
    {
        uart_Temp = (uint8_t)UART_RXD_Receive(UART_CH);
        UART_RXD_Flag_Clear(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&uart_Temp);
        }
    }
    if (usart_flag_get(Temp_USART, USART_IDLEF_FLAG) != RESET)
    {
        uart_Temp = (uint8_t)UART_RXD_Receive(UART_CH);
        (void)uart_Temp;
        DMA_RX_IDLE_Clear(UART_CH);
	#if DMA_UART3_RX & DMA_UART
        Uart_DMA_RX_Switch(UART_CH, &Caven_Double_U3, 0);
	#endif
    }
}

	#if DMA_UART3_RX & DMA_UART
void DMA1_CH3_HANDLERIT ()
{
    UART_mType UART_CH = m_UART_CH3;
    if (dma_flag_get(DMA1_FDT3_FLAG) != RESET)
    {
        dma_flag_clear(DMA1_FDT3_FLAG);
        Uart_DMA_RX_Switch(UART_CH, &Caven_Double_U3, 1);
    }
}
	#endif
#endif

#if (Exist_UART & OPEN_10000)
void Uart4_Init(int Baud, int Set)
{
    gpio_init_type GPIO_InitStructure = {0};
    usart_init_type USART_InitStructure = {0};
    nvic_init_type NVIC_InitStructure = {0};
    usart_type *Temp_USART = UART4;
    confirm_state temp;

    uart4_enable = 0;
    uart4_dma_enable = 0;
    temp = (Set != 0) ? ENABLE : DISABLE;

    crm_periph_clock_enable(CRM_GPIOF_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_UART4_PERIPH_CLOCK, temp);
    usart_reset(Temp_USART);

    gpio_default_para_init(&GPIO_InitStructure);
    GPIO_InitStructure.gpio_pins = GPIO_PINS_5;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.gpio_pins = GPIO_PINS_4;
    GPIO_InitStructure.gpio_mode = GPIO_MODE_MUX;
    GPIO_InitStructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
    GPIO_InitStructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(GPIOF, &GPIO_InitStructure);
    if (!temp)
    {
        GPIO_InitStructure.gpio_pins = GPIO_PINS_4 | GPIO_PINS_5;
        GPIO_InitStructure.gpio_mode = GPIO_MODE_INPUT;
        gpio_init(GPIOF, &GPIO_InitStructure);
    }

    usart_init(Temp_USART, Baud, USART_DATA_8BITS, USART_STOP_1_BIT);	// 波特率、位数、停止位
    usart_transmitter_enable(Temp_USART, temp);							// 发送使能
    usart_receiver_enable(Temp_USART, temp);							// 接收使能
    usart_parity_selection_config(Temp_USART, USART_PARITY_NONE);		// 无奇偶校验
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
#if DMA_UART4_RX & DMA_UART
    if (temp)
    {
        Uart_DMA_RX_Switch(1, &Caven_Double_U4, 0);
        uart4_dma_enable |= UART_DMA_RX_ENABLE;
    }
	usart_interrupt_enable(Temp_USART, USART_IDLE_INT, temp);
	//
#else
    usart_interrupt_enable(Temp_USART, USART_RDBF_INT, temp);
#endif
#if DMA_UART
    uart4_dma_enable |= UART_DMA_TX_ENABLE;
#endif
    if (Set == 0)
    {
        uart4_dma_enable = 0;
    }
    nvic_irq_enable(UART4_IRQn, 0, 4);
	
    usart_enable(Temp_USART, temp);
    uart4_enable = (uint8_t)Set;
}

void UART4_HANDLERIT ()
{
    uint8_t uart_Temp;
    UART_mType UART_CH = 4;
    usart_type *Temp_USART = UART4;

    if (UART_RXD_Flag(UART_CH))
    {
        uart_Temp = (uint8_t)UART_RXD_Receive(UART_CH);
        UART_RXD_Flag_Clear(UART_CH);
        if (State_Machine_UART_pFun[UART_CH] != NULL)
        {
            State_Machine_UART_pFun[UART_CH](&uart_Temp);
        }
    }
    if (usart_flag_get(Temp_USART, USART_IDLEF_FLAG) != RESET)
    {
        uart_Temp = (uint8_t)UART_RXD_Receive(UART_CH);
        (void)uart_Temp;
        DMA_RX_IDLE_Clear(UART_CH);
#if DMA_UART4_RX & DMA_UART
        Uart_DMA_RX_Switch(UART_CH, &Caven_Double_U4, 0);
#endif
    }
}
#endif

int Base_UART_Init(UART_mType Channel, int Baud, int Set)
{
    int retval = -1;
#if Exist_UART
    int Baud_CK = Baud & 0x00FFFFFF;
#if DMA_UART
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
#endif
    switch (Channel)
    {
    case 0:
        break;
    case 1:
#if (Exist_UART & OPEN_0010)
        Uart1_Init(Baud_CK, Set);
        retval = 0;
#endif
        break;
    case 2:
#if (Exist_UART & OPEN_0100)
        Uart2_Init(Baud_CK, Set);
        retval = 0;
#endif
        break;
    case 3:
#if (Exist_UART & OPEN_1000)
        Uart3_Init(Baud_CK, Set);
        retval = 0;
#endif
        break;
    case 4:
#if (Exist_UART & OPEN_10000)
        Uart4_Init(Baud_CK, Set);
        retval = 0;
#endif
        break;
    default:
        break;
    }
#endif
    return retval;
}

int State_Machine_Bind(UART_mType Channel, iD_pFun UART_pFun)
{
    int retval = -1;
#if Exist_UART
    if (UART_pFun == NULL ||
        (uint32_t)Channel >= (sizeof(State_Machine_UART_pFun) / sizeof(State_Machine_UART_pFun[0])))
    {
        return retval;
    }
    State_Machine_UART_pFun[Channel] = UART_pFun;
    retval = 0;
#endif
    return retval;
}

void Base_UART_Recv_Poll_Task(void)
{
#if Exist_UART
    Caven_DoubleBufType *temp_DoubleBuf = NULL;
    UART_mType UART_CH = m_UART_CH0;
    int temp_num = 0, temp_flag = 0, temp_run = 0;
    uint8_t *temp_cache;

    if ((uart0_dma_enable & UART_DMA_RX_ENABLE) > 0)
    {
    }
    if ((uart1_dma_enable & UART_DMA_RX_ENABLE) > 0)
    {
#if DMA_UART1_RX & DMA_UART
        temp_DoubleBuf = &Caven_Double_U1;
        UART_CH = 1;
        temp_num = temp_DoubleBuf->buf_len[temp_DoubleBuf->r_buf_idx];
        temp_cache = temp_DoubleBuf->buf[temp_DoubleBuf->r_buf_idx];
        temp_run = temp_DoubleBuf->r_buf_run;
        if (temp_DoubleBuf->buf_flag[temp_DoubleBuf->r_buf_idx] > 0)
        {
            int i = 0;
            if (State_Machine_UART_pFun[UART_CH] != NULL)
            {
                for (i = temp_run; i < temp_num; i++)
                {
                    temp_flag = State_Machine_UART_pFun[UART_CH](temp_cache + i);
                    if (temp_flag == 1)
                    {
                        temp_DoubleBuf->r_buf_run = i + 1;
                        break;
                    }
                }
            }
            else
            {
                i = temp_num;
            }
            if (i >= temp_num)
            {
                temp_DoubleBuf->buf_flag[temp_DoubleBuf->r_buf_idx] = 0;
                temp_DoubleBuf->buf_len[temp_DoubleBuf->r_buf_idx] = 0;
                temp_DoubleBuf->r_buf_run = 0;
                if (temp_DoubleBuf->w_buf_event)
                {
                    Uart_DMA_RX_Switch(UART_CH, temp_DoubleBuf, 0);
                }
            }
        }
#endif
    }
    if ((uart2_dma_enable & UART_DMA_RX_ENABLE) > 0)
    {
#if DMA_UART2_RX & DMA_UART
        temp_DoubleBuf = &Caven_Double_U2;
        UART_CH = m_UART_CH2;
        temp_num = temp_DoubleBuf->buf_len[temp_DoubleBuf->r_buf_idx];
        temp_cache = temp_DoubleBuf->buf[temp_DoubleBuf->r_buf_idx];
        temp_run = temp_DoubleBuf->r_buf_run;
        if (temp_DoubleBuf->buf_flag[temp_DoubleBuf->r_buf_idx] > 0)
        {
            int i = 0;
            if (State_Machine_UART_pFun[UART_CH] != NULL)
            {
                for (i = temp_run; i < temp_num; i++)
                {
                    temp_flag = State_Machine_UART_pFun[UART_CH](temp_cache + i);
                    if (temp_flag == 1)
                    {
                        temp_DoubleBuf->r_buf_run = i + 1;
                        break;
                    }
                }
            }
            else
            {
                i = temp_num;
            }
            if (i >= temp_num)
            {
                temp_DoubleBuf->buf_flag[temp_DoubleBuf->r_buf_idx] = 0;
                temp_DoubleBuf->buf_len[temp_DoubleBuf->r_buf_idx] = 0;
                temp_DoubleBuf->r_buf_run = 0;
                if (temp_DoubleBuf->w_buf_event)
                {
                    Uart_DMA_RX_Switch(UART_CH, temp_DoubleBuf, 0);
                }
            }
        }
#endif
    }
    if ((uart3_dma_enable & UART_DMA_RX_ENABLE) > 0)
    {
#if DMA_UART3_RX & DMA_UART
        temp_DoubleBuf = &Caven_Double_U3;
        UART_CH = m_UART_CH3;
        temp_num = temp_DoubleBuf->buf_len[temp_DoubleBuf->r_buf_idx];
        temp_cache = temp_DoubleBuf->buf[temp_DoubleBuf->r_buf_idx];
        temp_run = temp_DoubleBuf->r_buf_run;
        if (temp_DoubleBuf->buf_flag[temp_DoubleBuf->r_buf_idx] > 0)
        {
            int i = 0;
            if (State_Machine_UART_pFun[UART_CH] != NULL)
            {
                for (i = temp_run; i < temp_num; i++)
                {
                    temp_flag = State_Machine_UART_pFun[UART_CH](temp_cache + i);
                    if (temp_flag == 1)
                    {
                        temp_DoubleBuf->r_buf_run = i + 1;
                        break;
                    }
                }
            }
            else
            {
                i = temp_num;
            }
            if (i >= temp_num)
            {
                temp_DoubleBuf->buf_flag[temp_DoubleBuf->r_buf_idx] = 0;
                temp_DoubleBuf->buf_len[temp_DoubleBuf->r_buf_idx] = 0;
                temp_DoubleBuf->r_buf_run = 0;
                if (temp_DoubleBuf->w_buf_event)
                {
                    Uart_DMA_RX_Switch(UART_CH, temp_DoubleBuf, 0);
                }
            }
        }
#endif
    }
#endif
}
