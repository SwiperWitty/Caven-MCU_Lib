#include "Base_UART.h" 

#ifdef Exist_UART

#define RXD_Falg    USART_INT_FLAG_RBNE     //  接收标志
#define TXD_Falg    USART_FLAG_TC		    //  【USART_FLAG_TXE】这个只是说明，数据被cpu取走,【USART_FLAG_TC】这是完全发送完成

static uint32_t uart_Temp;
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
        uart_Temp = USART0;
        break;
    case 1:
        uart_Temp = USART1;
        break;
    case 2:
        uart_Temp = USART2;
        break;
    case 3:
        uart_Temp = UART3;
        break;
    case 4:
        uart_Temp = UART4;
        break;
    default:
        return 0;
    }
    res = usart_interrupt_flag_get(uart_Temp,RXD_Falg);
    return res;
}

static void UART_RXD_Flag_Clear(UART_mType Channel)
{
    switch (Channel)
    {
    case 0:
        uart_Temp = USART0;
        break;
    case 1:
        uart_Temp = USART1;
        break;
    case 2:
        uart_Temp = USART2;
        break;
    case 3:
        uart_Temp = UART3;
        break;
    case 4:
        uart_Temp = UART4;
        break;
    default:
        return;
    }
    usart_interrupt_flag_clear(uart_Temp, RXD_Falg);
}

/*  发送 接收    */

// 接收
static uint16_t UART_RXD_Receive(UART_mType Channel)     //RXD 读取值
{
    uint16_t res;
    switch (Channel)
    {
    case 0:
        uart_Temp = USART0;
        break;
    case 1:
        uart_Temp = USART1;
        break;
    case 2:
        uart_Temp = USART2;
        break;
    case 3:
        uart_Temp = UART3;
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

// 发送
void Base_UART_Send_Data(UART_mType Channel,uint16_t data)
{
    switch (Channel)
    {
    case 0:
        if (uart0_enable == 0)
        {
            return;
        }
        uart_Temp = USART0;
        break;
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
        uart_Temp = UART3;
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
    
    while (usart_flag_get(uart_Temp, TXD_Falg) == 0);
    usart_flag_clear(uart_Temp, TXD_Falg);
    usart_data_transmit(uart_Temp, data);
}

void Base_UART_DMA_Send_Data(UART_mType Channel,const uint8_t *data,int Length)
{
    
}

#endif


#if (Exist_UART & OPEN_0001)
void Uart0_Init(int Baud,int Set)
{
    uart_Temp = USART0;
    uart0_enable = Set;

    if(Set)
    {
        rcu_periph_clock_enable(RCU_GPIOA);
        /* enable USART clock */
        rcu_periph_clock_enable(RCU_USART0);
        
        gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_9);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_9);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_10);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);
        
        usart_deinit(uart_Temp);
        usart_baudrate_set(uart_Temp,Baud);
        usart_parity_config(uart_Temp, USART_PM_NONE);
        usart_word_length_set(uart_Temp, USART_WL_8BIT);
        usart_stop_bit_set(uart_Temp, USART_STB_1BIT);
        usart_halfduplex_disable(uart_Temp);
        
        usart_receive_config(uart_Temp, USART_RECEIVE_ENABLE);
        usart_transmit_config(uart_Temp, USART_TRANSMIT_ENABLE);
        
        usart_interrupt_enable(uart_Temp, USART_INT_RBNE);
        usart_enable(uart_Temp);
        
        nvic_irq_enable(USART0_IRQn, 0, 1);
    }
    else
    {
        rcu_periph_clock_disable(RCU_USART0);
        nvic_irq_disable(USART0_IRQn);
    }
}

void UART0_HANDLERIT()
{
    uint16_t temp;
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
    uart_Temp = USART1;
    uart1_enable = Set;

    if(Set)
    {
        rcu_periph_clock_enable(RCU_GPIOA);
        /* enable USART clock */
        rcu_periph_clock_enable(RCU_USART1);
        
        gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_2 | GPIO_PIN_3);
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_2);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_2);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_3);
        gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_3);
        
        usart_deinit(uart_Temp);
        usart_baudrate_set(uart_Temp,Baud);
        usart_parity_config(uart_Temp, USART_PM_NONE);
        usart_word_length_set(uart_Temp, USART_WL_8BIT);
        usart_stop_bit_set(uart_Temp, USART_STB_1BIT);
        usart_halfduplex_disable(uart_Temp);
        
        usart_receive_config(uart_Temp, USART_RECEIVE_ENABLE);
        usart_transmit_config(uart_Temp, USART_TRANSMIT_ENABLE);
        
        usart_interrupt_enable(uart_Temp, USART_INT_RBNE);
        usart_enable(uart_Temp);
        
        nvic_irq_enable(USART1_IRQn, 1, 1);
    }
    else
    {
        rcu_periph_clock_disable(RCU_USART1);
        nvic_irq_disable(USART1_IRQn);
    }
}

void UART1_HANDLERIT()
{
    uint16_t temp;
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
    uart_Temp = USART2;
    uart2_enable = Set;

    if(Set)
    {
        rcu_periph_clock_enable(RCU_GPIOC);
        /* enable USART clock */
        rcu_periph_clock_enable(RCU_USART2);
        
        gpio_af_set(GPIOC, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_9);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_9);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_10);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);
        
        usart_deinit(uart_Temp);
        usart_baudrate_set(uart_Temp,Baud);
        usart_parity_config(uart_Temp, USART_PM_NONE);
        usart_word_length_set(uart_Temp, USART_WL_8BIT);
        usart_stop_bit_set(uart_Temp, USART_STB_1BIT);
        usart_halfduplex_disable(uart_Temp);
        
        usart_receive_config(uart_Temp, USART_RECEIVE_ENABLE);
        usart_transmit_config(uart_Temp, USART_TRANSMIT_ENABLE);
        
        usart_interrupt_enable(uart_Temp, USART_INT_RBNE);
        usart_enable(uart_Temp);
        
        nvic_irq_enable(USART2_IRQn, 1, 1);
    }
    else
    {
        rcu_periph_clock_disable(RCU_USART2);
        nvic_irq_disable(USART2_IRQn);
    }
}

void UART2_HANDLERIT()
{
    uint16_t temp;
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
    uart_Temp = UART3;
    uart3_enable = Set;

    if(Set)
    {
        rcu_periph_clock_enable(RCU_GPIOC);
        /* enable USART clock */
        rcu_periph_clock_enable(RCU_UART3);
        
        gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_9 | GPIO_PIN_10);
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_9);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_9);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_10);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_10);
        
        usart_deinit(uart_Temp);
        usart_baudrate_set(uart_Temp,Baud);
        usart_parity_config(uart_Temp, USART_PM_NONE);
        usart_word_length_set(uart_Temp, USART_WL_8BIT);
        usart_stop_bit_set(uart_Temp, USART_STB_1BIT);
        usart_halfduplex_disable(uart_Temp);
        
        usart_receive_config(uart_Temp, USART_RECEIVE_ENABLE);
        usart_transmit_config(uart_Temp, USART_TRANSMIT_ENABLE);
        
        usart_interrupt_enable(uart_Temp, USART_INT_RBNE);
        usart_enable(uart_Temp);
        
        nvic_irq_enable(UART3_IRQn, 1, 1);
    }
    else
    {
        rcu_periph_clock_disable(RCU_UART3);
        nvic_irq_disable(UART3_IRQn);
    }
}

void UART3_HANDLERIT()
{
    uint16_t temp;
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
    uart_Temp = UART4;
    uart4_enable = Set;

    if(Set)
    {
        rcu_periph_clock_enable(RCU_GPIOC);
        rcu_periph_clock_enable(RCU_GPIOD);
        /* enable USART clock */
        rcu_periph_clock_enable(RCU_UART4);
        
        gpio_af_set(GPIOC, GPIO_AF_8, GPIO_PIN_12);
        gpio_af_set(GPIOD, GPIO_AF_8, GPIO_PIN_2);
        gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_12);
        gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_12);

        /* configure USART Rx as alternate function push-pull */
        gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_2);
        gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_2);
        
        usart_deinit(uart_Temp);
        usart_baudrate_set(uart_Temp,Baud);
        usart_parity_config(uart_Temp, USART_PM_NONE);
        usart_word_length_set(uart_Temp, USART_WL_8BIT);
        usart_stop_bit_set(uart_Temp, USART_STB_1BIT);
        usart_halfduplex_disable(uart_Temp);
        
        usart_receive_config(uart_Temp, USART_RECEIVE_ENABLE);
        usart_transmit_config(uart_Temp, USART_TRANSMIT_ENABLE);
        
        usart_interrupt_enable(uart_Temp, USART_INT_RBNE);
        usart_enable(uart_Temp);
        
        nvic_irq_enable(UART4_IRQn, 1, 1);
    }
    else
    {
        rcu_periph_clock_disable(RCU_UART4);
        nvic_irq_disable(UART4_IRQn);
    }
}

void UART4_HANDLERIT()
{
    uint16_t temp;
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
	
#ifdef Exist_UART
    switch (Channel)
    {
    case 0:
    #if (Exist_UART & OPEN_0001)
        Uart0_Init(Baud, Set);
        retval = 0;
    #endif
        break;
    case 1:
    #if (Exist_UART & OPEN_0010)
        Uart1_Init(Baud, Set);
        retval = 0;
    #endif
        break;
    case 2:
    #if (Exist_UART & OPEN_0100)
        Uart2_Init(Baud, Set);
        retval = 0;
    #endif
        break;
    case 3:
    #if (Exist_UART & OPEN_1000)
        Uart3_Init(Baud, Set);
        retval = 0;
    #endif
        break;
    case 4:
    #if (Exist_UART & OPEN_10000)
        Uart4_Init(Baud, Set);
        retval = 0;
    #endif
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
    retval = 0;
#endif
    return retval;
}

int fputc(int ch, FILE *f)      //printf
{
#ifdef DEBUG_CH
    #ifdef Exist_UART
    Base_UART_Send_Data((UART_mType)DEBUG_CH,(uint8_t)ch);
    #endif
#endif // DEBUG
    return (ch);
}

// 你找中断？UART的中断通过函数回调给MODE了！ 
