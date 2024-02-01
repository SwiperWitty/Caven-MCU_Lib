#include "Base_SPI.h"

#ifdef Exist_SPI

    #if (SPIx == 1)
        spi_type *Temp_SPI = SPI1;
        #define SPI_Tx_DMA_Channel     DMA1_CHANNEL3
    #elif (SPIx == 2)
        spi_type *Temp_SPI = SPI2;
        #define SPI_Tx_DMA_Channel     DMA1_CHANNEL5
    #endif
    spi_type *spi_Temp;
    
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
    
#endif

__IO uint16_t SPI_complete_flag = 1;


void SPI1_GPIO_Init(int Set)
{
#ifdef Exist_SPI
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

void SPI2_GPIO_Init(int Set)
{
#ifdef Exist_SPI
    gpio_init_type gpio_init_struct;
    gpio_default_para_init(&gpio_init_struct);
    if (Set)
    {
        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK,TRUE);
        crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
        
        gpio_init_struct.gpio_pins = SPI2_SCK | SPI2_MOSI;      //SCK、MOSI输出模式（复用）
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
#ifdef Exist_SPI
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
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
    spi_init(spi_Temp, &spi_init_struct);
    
    spi_enable(spi_Temp, set);
#endif
}

void Base_SPI2_Init (uint8_t Width,int Set)
{
#ifdef Exist_SPI
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
    spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
    spi_init(spi_Temp, &spi_init_struct);
    
    spi_enable(spi_Temp, set);
#endif
}

//普通\软件 发送，只管SCLK、MOSI不管 NSS
void SPI_Send_DATA(const uint16_t DATA)     
{
    /*
     * 写标志位
     * 写数据
     * 等忙位
     */
#ifdef Exist_SPI
    #ifdef SPI_Software
    char temp;
    for (int i = 0; i < 8; i++)
    {
        SPI_SCK_L();           //预备Down沿
        temp = (DATA << i) & 0x80;
        if (temp)
            SPI_MOSI_H();      //数据1
        else
            SPI_MOSI_L();      //数据0
        SPI_SCK_H();           //完成Down沿
    }
    SPI_MOSI_H();
    //    SPI_SCK_H();              // 0 / 0不需要上升
    #else

//    spi_i2s_data_transmit(Temp_SPI, DATA);
//    while(spi_i2s_flag_get(Temp_SPI,SPI_I2S_BF_FLAG) != RESET);   //SPI忙就会 = 1，不忙就是0
    
//    while(spi_i2s_flag_get(Temp_SPI,SPI_I2S_TDBE_FLAG) == RESET);   //发送缓冲区空了
//    spi_i2s_data_transmit(Temp_SPI, DATA);
//    while(spi_i2s_flag_get(Temp_SPI,SPI_I2S_BF_FLAG) != RESET);   //SPI忙就会 = 1，不忙就是0
    
    #endif
	
#endif
}


void SPI_SET_Addr_SendData(char Serial,uint16_t Addr,uint16_t DATA)
{
// Addr &= 0xBFFF;
#ifdef Exist_SPI
//    SPI_CS_Set(Serial,1);      //SPI开始（片选）
//    SPI_Delay (1);

//    SPI_Send_DATA(Addr);
////    SPI_Delay (1);
//    SPI_Send_DATA(DATA);

//    SPI_Delay (1);
    
#endif
}

#ifdef Exist_SPI
void SPI1_IRQHandler(void)
{
    if (spi_i2s_flag_get(SPI1, SPI_I2S_TDBE_FLAG) == 1)
    {
        if(spi_i2s_flag_get(SPI1,SPI_I2S_BF_FLAG) == 0)     //busy
        {
            spi_i2s_interrupt_enable(SPI1, SPI_I2S_TDBE_INT, FALSE);
//            SPI_CS_Set(1,FALSE);          //取消片选
//            SPI_complete_flag = 1;
//            printf("SPI ok \r\n");
        }
    }
}

 void SPI2_IRQHandler(void)
{
    if (spi_i2s_flag_get(SPI2, SPI_I2S_TDBE_FLAG) == 1)
    {
        if(spi_i2s_flag_get(SPI2,SPI_I2S_BF_FLAG) == 0)
        {
//            spi_i2s_interrupt_enable(SPI2, SPI_I2S_TDBE_INT, FALSE);    //关中断
//            SPI_CS_Set(1,FALSE);          //取消片选
//            SPI_complete_flag = 1;
        }
    }
}
#endif

int Base_SPI_Init(SPI_mType Channel,uint8_t Width,int Set)
{
    int retval = -1;
	
#ifdef Exist_SPI
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
    crm_periph_clock_enable(CRM_DMA2_PERIPH_CLOCK, TRUE);
    switch (Channel)
    {
    case 0:
		
        break;
    case 1:
        SPI1_GPIO_Init(Set);
        Base_SPI1_Init(Width, Set);
        retval = 0;
        break;
    case 2:
        SPI2_GPIO_Init(Set);
        Base_SPI2_Init(Width, Set);
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
        if(Serial == 1)
        {
            if(State) {SPI2_NSS_H();}
            else {SPI2_NSS_L();}
        }
        break;
    case 2:
        if(Serial == 1)
        {
            if(State) {SPI2_NSS_H();}
            else {SPI2_NSS_L();}
        }
        break;
    case 3:

        break;
    default:
        break;
    }
#endif
    
}

void Base_SPI_ASK_Receive(SPI_mType Channel,uint16_t Data,uint16_t *Receive)
{
#ifdef Exist_SPI
    
#endif 
}

void Base_SPI_Send_Data(SPI_mType Channel,uint16_t Data)
{
#ifdef Exist_SPI
    
#endif 
}

void Base_SPI_DMA_Send_Data(SPI_mType Channel,const uint8_t *Data_array,int Length)
{
#ifdef SPI_DMA
    
#endif 
}
