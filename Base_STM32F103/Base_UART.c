#include "Base_UART.h" 

#ifdef Exist_UART
static usart_type * Temp;

#endif

void Uart1_Init(int Baud,int Set)
{
#ifdef UART1_EXIST
    confirm_state set = FALSE;
    Temp = USART1;
    usart_reset(Temp);
    if (Set)
        set = TRUE;

    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, set);
    crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_9;                           //Tx
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init(GPIOA, &gpio_init_struct);

    gpio_init_struct.gpio_pins = GPIO_PINS_10;                           //Rx
    gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOA, &gpio_init_struct);

    nvic_priority_group_config(NVIC_PRIORITY_GROUP_0);
    nvic_irq_enable(USART1_IRQn, 0, 1);

    usart_init(Temp, Baud, USART_DATA_8BITS, USART_STOP_1_BIT);   //波特率、位数、停止位
    usart_transmitter_enable(Temp, TRUE);         //发送使能
    usart_receiver_enable(Temp, TRUE);            //接收使能

    usart_parity_selection_config(Temp,USART_PARITY_NONE);    //无奇偶校验
    usart_interrupt_enable(Temp, USART_RDBF_INT, TRUE);
    usart_enable(Temp, TRUE);
#endif
}

void Uart2_Init(int Baud,int Set)
{    
#ifdef UART2_EXIST

#endif
}

void Uart3_Init(int Baud,int Set)
{
#ifdef UART3_EXIST

#endif
}

void Uart4_Init(int Baud,int Set)
{
#ifdef UART4_EXIST

#endif
}

void Uart5_Init(int Baud,int Set)
{
#ifdef UART5_EXIST
    confirm_state set = FALSE;
    Temp = UART4;
    usart_reset(Temp);
    if (Set)
        set = TRUE;
    crm_periph_clock_enable(CRM_UART4_PERIPH_CLOCK, set);
    crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);

    gpio_pin_remap_config(SWJTAG_GMUX_010,TRUE);                //支持 SWD，禁用 JTAG，PA15/PB3/PB4 可作GPIO
#endif
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
#ifdef UART2_EXIST
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
        Temp = UART4;
#endif
        break;
    case 5:
#ifdef UART5_EXIST
        Temp = UART5;
#endif
        break;
    default:
        return (0);
    }
#ifdef Exist_UART
    res = usart_flag_get(Temp,RXD_Falg);
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
        Temp = UART4;
#endif
        break;
    case 5:
#ifdef UART5_EXIST
        Temp = UART5;
#endif
        break;
    default:
        return;
    }
#ifdef Exist_UART
    usart_flag_clear(Temp, RXD_Falg);
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
        Temp = UART4;
#endif
        break;
    case 5:
#ifdef UART5_EXIST
        Temp = UART5;
#endif
        break;
    default:
        break;
    }
    #ifdef Exist_UART
    res = usart_data_receive(Temp);
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
        Temp = UART4;
#endif
        break;
//UART4
    case 5:
#ifdef UART5_EXIST
        Temp = UART5;
#endif
        break;
//UART5
    default:
        return;
//error,直接返回
    }
#ifdef Exist_UART
    while (usart_flag_get(Temp, TXD_Falg) == RESET);  
    usart_data_transmit(Temp, DATA);
    // usart_flag_clear(Temp, TXD_Falg);        //可以不要
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

