#include "Base_SPI.h"

#ifdef Exist_SPI

    spi_type *spi_Temp;
    char SPI1_Width = 0;
    char SPI2_Width = 0;

static void SPI_Delay (int time)
{
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = 10;            //SET
        do{
            NOP();
        }while((temp--) > 0);
    }
}


static void SPI1_GPIO_Init(int Set)
{
#if Exist_SPI & OPEN_0010
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK,TRUE);
        crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);

        gpio_init_struct.gpio_pins = SPI1_SCK | SPI1_MOSI;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = SPI_MODE_OUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIO_SPI1, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = SPI1_NSS;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;      // NSS-输出模式（输出）
        gpio_init(GPIO_SPI1, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = SPI1_MISO;
        gpio_init_struct.gpio_mode = SPI_MODE_IN;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(GPIO_SPI1, &gpio_init_struct);
        
        SPI1_NSS_H();
    }
    else
    {
        gpio_init_struct.gpio_pins = SPI1_NSS|SPI1_SCK|SPI1_MOSI|SPI1_MISO;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIO_SPI1, &gpio_init_struct);
    }
#endif
}

static void SPI2_GPIO_Init(int Set)
{
#if Exist_SPI & OPEN_0100
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);
        crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
        
        gpio_init_struct.gpio_pins = SPI2_SCK | SPI2_MOSI;
        gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
        gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
        gpio_init_struct.gpio_mode = SPI_MODE_OUT;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIO_SPI2, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = SPI2_NSS;
        gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;      // NSS-输出模式（输出）
        gpio_init(GPIO_SPI2, &gpio_init_struct);
        
        gpio_init_struct.gpio_pins = SPI2_MISO;
        gpio_init_struct.gpio_mode = SPI_MODE_IN;
        gpio_init_struct.gpio_pull = GPIO_PULL_UP;
        gpio_init(GPIO_SPI2, &gpio_init_struct);
        
        SPI2_NSS_H();
    }
    else
    {
        gpio_init_struct.gpio_pins = SPI2_NSS|SPI2_SCK|SPI2_MOSI|SPI2_MISO;
        gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
        gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
        gpio_init(GPIO_SPI2, &gpio_init_struct);
    }
#endif
}

/*
    这个是给硬件SPI的初始化 
*/
void Base_SPI1_Init (uint8_t Width,int Set)
{
    confirm_state set = FALSE;
    spi_init_type spi_init_struct;
    spi_default_para_init(&spi_init_struct);
    
    spi_Temp = SPI1;
    if (Set)
        set = TRUE;
    spi_i2s_reset(spi_Temp);
    crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);

    #ifdef HOST_MODE
    spi_init_struct.master_slave_mode =SPI_MODE_MASTER;
    #else
    spi_init_struct.master_slave_mode =SPI_MODE_SLAVE;
    #endif
    if(Width > 8)
    {
        spi_init_struct.frame_bit_num = SPI_FRAME_16BIT;
    }
    else
    {
        spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
    }
    spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;           // 闲时 SCK状态
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;                // 上升沿让从机读
    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;           // 软件cs
    spi_init_struct.mclk_freq_division = SPI_SPEED;                     // 速度
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;		// SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    spi_init(spi_Temp, &spi_init_struct);
    
    #ifdef SPI_DMA
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
	//
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(DMA1_Channel3_IRQn, 1, 1);
    dma_reset(DMA1_CHANNEL3);                             // 优先级
    dma_interrupt_enable(DMA1_CHANNEL3, DMA_FDT_INT, TRUE);
    #endif
    spi_enable(spi_Temp, set);
}

void Base_SPI2_Init (uint8_t Width,int Set)
{
    confirm_state set = FALSE;
    spi_init_type spi_init_struct;
    spi_default_para_init(&spi_init_struct);
    
    spi_Temp = SPI2;
    if (Set)
        set = TRUE;
    spi_i2s_reset(spi_Temp);
    crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
    
    #ifdef HOST_MODE
    spi_init_struct.master_slave_mode =SPI_MODE_MASTER;
    #else
    spi_init_struct.master_slave_mode =SPI_MODE_SLAVE;
    #endif
    if(Width > 8)
    {
        spi_init_struct.frame_bit_num = SPI_FRAME_16BIT;
    }
    else
    {
        spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
    }
    spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
    spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;           // 闲时 SCK状态
    spi_init_struct.clock_phase = SPI_CLOCK_PHASE_2EDGE;                // 上升沿让从机读
    spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;           // 软件cs
    spi_init_struct.mclk_freq_division = SPI_SPEED;                     // 速度
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;		// SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    spi_init(spi_Temp, &spi_init_struct);

    #ifdef SPI_DMA
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    /* enable iomux periph clock */
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
	//
    nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
    nvic_irq_enable(DMA1_Channel5_IRQn, 1, 2);
    dma_reset(DMA1_CHANNEL5);                             // 优先级
    dma_interrupt_enable(DMA1_CHANNEL5, DMA_FDT_INT, TRUE);
    #endif
    spi_enable(spi_Temp, set);

}
#endif

int Base_SPI_Init(SPI_mType Channel,uint8_t Width,int Set)
{
    int retval = -1;
#ifdef Exist_SPI

    SPI_Delay (1);
    switch (Channel)
    {
    case 0:
		
        break;
    case 1:
        SPI1_GPIO_Init(Set);
    #ifndef SPI_SOFTWARE
        Base_SPI1_Init(Width, Set);
    #endif
        SPI1_Width = Width;
        retval = 0;
        break;
    case 2:
        SPI2_GPIO_Init(Set);
    #ifndef SPI_SOFTWARE
        Base_SPI2_Init(Width, Set);
    #endif
        SPI2_Width = Width;
        retval = 0;
        break;
    case 3:

        break;
    default:
        break;
    }
#endif
    return retval;
}

void Base_SPI_CS_Set(SPI_mType Channel,char Serial,char State)
{
#ifdef Exist_SPI
    
    switch (Channel)
    {
    case 0:

        break;
    case 1:
    #if Exist_SPI & OPEN_0010
        if(Serial == 1)
        {
            if(State) {SPI1_NSS_L();}
            else {SPI1_NSS_H();}
        }
    #endif
        break;
    case 2:
    #if Exist_SPI & OPEN_0100
        if(Serial == 1)
        {
            if(State) {SPI2_NSS_L();}
            else {SPI2_NSS_H();}
        }
    #endif
        break;
    case 3:

        break;
    default:
        break;
    }
#endif
    
}

void Base_SPI_Send_Data(SPI_mType Channel,uint16_t Data)
{
#ifdef Exist_SPI
    /*
     * 写标志位
     * 写数据
     * 等忙位
     */
    #ifdef SPI_SOFTWARE
    int temp;
    switch (Channel)
    {
    case 0:

        break;
    case 1:
        #if Exist_SPI & OPEN_0010
        for (int i = 0; i < SPI1_Width; i++)
        {
            SPI1_SCK_L();           //预备Down沿
            temp = (Data << i) & (0x01 << (SPI1_Width - 1));
            if (temp)
                SPI1_MOSI_H();      //数据1
            else
                SPI1_MOSI_L();      //数据0
            SPI1_SCK_H();           //完成Down沿
        }
        SPI1_MOSI_H();
        #endif
        break;
    case 2:
        #if Exist_SPI & OPEN_0100
        for (int i = 0; i < SPI2_Width; i++)
        {
            SPI2_SCK_L();           //预备Down沿
            temp = (Data << i) & ((u16)0x01 << (SPI2_Width - 1));
            if (temp)
                SPI2_MOSI_H();      //数据1
            else
                SPI2_MOSI_L();      //数据0
            SPI2_SCK_H();           //完成Down沿
        }
        SPI2_MOSI_H();
        #endif
        break;
    case 3:

        break;
    default:
        break;
    }
    #else

    switch (Channel)
    {
    case 0:

        break;
    case 1:
    #if Exist_SPI & OPEN_0010
        spi_Temp = SPI1;
    #endif
        break;
    case 2:
    #if Exist_SPI & OPEN_0100
        spi_Temp = SPI2;
    #endif
        break;
    case 3:

        break;
    default:
        break;
    }
    while(spi_i2s_flag_get(spi_Temp, SPI_I2S_TDBE_FLAG) == RESET);   //发送缓冲区空了
    spi_i2s_data_transmit(spi_Temp, Data);
    while(spi_i2s_flag_get(spi_Temp,SPI_I2S_BF_FLAG) != RESET);     //SPI忙就会 = 1，不忙就是0
	
    #endif

#endif 
}

#ifdef SPI_DMA

    #if Exist_SPI & OPEN_0010
uint8_t DMA_SPI1_Buff[500];
volatile char SPI1_DMA_Flag = 0;
void SPI1_FINISH_HANDLERIT()
{
    if(dma_flag_get(DMA1_FDT3_FLAG) != RESET)       // 传输完成
    {
		if(spi_i2s_flag_get(SPI1,SPI_I2S_BF_FLAG) == RESET)
		{
			dma_flag_clear(DMA1_FDT3_FLAG);
			SPI1_DMA_Flag = 0;
			Base_SPI_CS_Set(m_SPI_CH1,1,0);
		}
    }
}
    #endif 
    #if Exist_SPI & OPEN_0100
uint8_t DMA_SPI2_Buff[500];
volatile char SPI2_DMA_Flag = 0;
void SPI2_FINISH_HANDLERIT()
{
    if(dma_flag_get(DMA1_FDT5_FLAG) != RESET)       // 传输完成
    {
		if(spi_i2s_flag_get(SPI2,SPI_I2S_BF_FLAG) == RESET)
		{
			dma_flag_clear(DMA1_FDT5_FLAG);
			SPI2_DMA_Flag = 0;
			Base_SPI_CS_Set(m_SPI_CH2,1,0);
		}
    }
}
    #endif 
#endif 



void Base_SPI_DMA_Send_Data(SPI_mType Channel,const void *Data_array,int size)
{
#ifdef SPI_DMA
    
    dma_init_type dma_init_struct;
    dma_channel_type *Temp_DMA_Channel;
    
//    uint32_t DMAy_FLAG;
    uint8_t *p_DMA_BUFF = NULL;
    volatile char *SPI_DMA_Flag_Temp = 0;
	dma_default_para_init(&dma_init_struct);
	dma_init_struct.buffer_size = size;
	
    switch (Channel)
    {
    case 0:

        break;
    case 1:
    #if Exist_SPI & OPEN_0010
        spi_Temp = SPI1;
        p_DMA_BUFF = DMA_SPI1_Buff;
        SPI_DMA_Flag_Temp = &SPI1_DMA_Flag;
        Temp_DMA_Channel = DMA1_CHANNEL3;
		if(SPI1_Width > 8)
		{
			dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
			dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
			dma_init_struct.buffer_size = size / 2;
		}
    #endif 
        break;
    case 2:
    #if Exist_SPI & OPEN_0100
        spi_Temp = SPI2;
        p_DMA_BUFF = DMA_SPI2_Buff;
        SPI_DMA_Flag_Temp = &SPI2_DMA_Flag;
        Temp_DMA_Channel = DMA1_CHANNEL5;
		if(SPI2_Width > 8)
		{
			dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
			dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
			dma_init_struct.buffer_size = size / 2;
		}
    #endif 
        break;
    case 3:

        break;
    default:
        break;
    }
    // 开始DMA
    if (Data_array == NULL || p_DMA_BUFF == NULL || (size < 0)) {
        return;
    }

    while((*SPI_DMA_Flag_Temp) != 0);
    memcpy(p_DMA_BUFF,Data_array,size);             // 一定等上一个发送完成才能修改
    *SPI_DMA_Flag_Temp = 1;
    Base_SPI_CS_Set(Channel,1,TRUE);
  
    dma_channel_enable(Temp_DMA_Channel, FALSE);
    
                                 //
    dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;       //
    dma_init_struct.memory_base_addr = (uint32_t)p_DMA_BUFF;        //
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)&(spi_Temp->dt);   //
    dma_init_struct.peripheral_inc_enable = FALSE;                      /*外设地址自增：关闭（一直是 SPI 数据寄存器，不变）*/
    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
    dma_init_struct.loop_mode_enable = FALSE;                           // 自动循环关
    dma_init(Temp_DMA_Channel, &dma_init_struct);
    
//    dma_flexible_config(DMA1, FLEX_CHANNEL5, DMA_FLEXIBLE_SPI2_TX);
    spi_i2s_dma_transmitter_enable(spi_Temp, TRUE);

    dma_channel_enable(Temp_DMA_Channel, TRUE);     /* begin dma transmitting */
#endif 
}

void Base_SPI_ASK_Receive(SPI_mType Channel,uint16_t Data,uint16_t *Receive)
{
#ifdef Exist_SPI
    
#endif 
}

