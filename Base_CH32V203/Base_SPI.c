#include "Base_SPI.h"
#include "Base_GPIO.h"

#ifdef Exist_SPI
SPI_TypeDef *spi_Temp = NULL;
char SPI1_Width = 0;
char SPI2_Width = 0;
char SPI1_Init = 0;
char SPI2_Init = 0;

void SPI_Delay (int time)
{
    int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = 12;            // SET
        do{
            NOP();
        }while((temp--) > 0);
    }
}

static void SPI1_GPIO_Init(int Set)
{
#if Exist_SPI & OPEN_0010
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

        gpio_init_struct.GPIO_Pin = SPI1_SCK | SPI1_MOSI;
        gpio_init_struct.GPIO_Mode = SPI_MODE_OUT;
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI1, &gpio_init_struct);

        gpio_init_struct.GPIO_Pin = SPI1_NSS;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIO_SPI1, &gpio_init_struct);

        gpio_init_struct.GPIO_Pin = SPI1_MISO;
        gpio_init_struct.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI1, &gpio_init_struct);

        SPI1_NSS_H();
    }
    else
    {
        gpio_init_struct.GPIO_Pin = SPI1_NSS|SPI1_SCK|SPI1_MOSI|SPI1_MISO;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIO_SPI1, &gpio_init_struct);
    }
#endif
}

static void SPI2_GPIO_Init(int Set)
{
#if Exist_SPI & OPEN_0100
    GPIO_InitTypeDef gpio_init_struct;
    GPIO_StructInit(&gpio_init_struct);
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

        gpio_init_struct.GPIO_Pin = SPI2_SCK | SPI2_MOSI;
        gpio_init_struct.GPIO_Mode = SPI_MODE_OUT;
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI2, &gpio_init_struct);

        gpio_init_struct.GPIO_Pin = SPI2_NSS;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;  // GPIO_Mode_Out_PP GPIO_Mode_AF_PP
        GPIO_Init(GPIO_SPI2, &gpio_init_struct);

        gpio_init_struct.GPIO_Pin = SPI2_MISO;
        gpio_init_struct.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI2, &gpio_init_struct);

        SPI2_NSS_H();
    }
    else
    {
        gpio_init_struct.GPIO_Pin = SPI2_NSS|SPI2_SCK|SPI2_MOSI|SPI2_MISO;
        gpio_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIO_SPI2, &gpio_init_struct);
    }
#endif
}

/*
    SPI初始化
*/
void Base_SPI1_Init (uint8_t Width,int Set)
{
    FunctionalState set = DISABLE;
    SPI_InitTypeDef spi_init_struct;
    spi_Temp = SPI1;
    if (Set)
        set = ENABLE;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, set);

    #ifdef HOST_MODE
    spi_init_struct.SPI_Mode = SPI_Mode_Master;
    #else
    spi_init_struct.SPI_Mode = SPI_Mode_Slave;
    #endif
    if(Width > 8)
    {
        spi_init_struct.SPI_DataSize = SPI_DataSize_16b;
    }
    else
    {
        spi_init_struct.SPI_DataSize = SPI_DataSize_8b;
    }
    spi_init_struct.SPI_FirstBit = SPI_FirstBit_MSB;
    spi_init_struct.SPI_CPOL = SPI_CPOL_High;           // 空闲高电平
    spi_init_struct.SPI_CPHA = SPI_CPHA_2Edge;          // 第二沿读取（上升沿）
    spi_init_struct.SPI_CRCPolynomial = 7;              // 
    spi_init_struct.SPI_NSS = SPI_NSS_Soft;             // SPI_NSS_Soft SPI_NSS_Hard
    spi_init_struct.SPI_BaudRatePrescaler = SPI_SPEED;                   //
    spi_init_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		// SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    SPI_Init(spi_Temp, &spi_init_struct);
    #if SPI_DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    #endif
    SPI_Cmd(spi_Temp, set);
}

void Base_SPI2_Init (uint8_t Width,int Set)
{
    FunctionalState set = DISABLE;
    SPI_InitTypeDef spi_init_struct;
    spi_Temp = SPI2;
    if (Set)
        set = ENABLE;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, set);

    #if HOST_MODE
    spi_init_struct.SPI_Mode = SPI_Mode_Master;
    #else
    spi_init_struct.SPI_Mode = SPI_Mode_Slave;
    #endif
    spi_init_struct.SPI_DataSize = SPI_DataSize_8b;
    if(Width > 8)
    {
        spi_init_struct.SPI_DataSize = SPI_DataSize_16b;
    }
    spi_init_struct.SPI_FirstBit = SPI_FirstBit_MSB;
    spi_init_struct.SPI_CPOL = SPI_CPOL_High;           // 空闲高电平
    spi_init_struct.SPI_CPHA = SPI_CPHA_2Edge;          // 第二沿读取（上升沿）
    spi_init_struct.SPI_CRCPolynomial = 7;              // 
    spi_init_struct.SPI_NSS = SPI_NSS_Soft;             // SPI_NSS_Soft SPI_NSS_Hard
    spi_init_struct.SPI_BaudRatePrescaler = SPI_SPEED;                   //
    spi_init_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		// SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    SPI_Init(spi_Temp, &spi_init_struct);
    #if SPI_DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    #endif
    SPI_Cmd(spi_Temp, set);
}
#endif

int Base_SPI_Init(SPI_mType Channel,uint8_t Width,int Set)
{
    int retval = -1;
#ifdef Exist_SPI
    switch (Channel)
    {
    case 0:
		
        break;
    case 1:
        spi_Temp = SPI1;
        SPI1_GPIO_Init(Set);
        Base_SPI1_Init(Width, Set);
        SPI1_Width = Width;
        SPI1_Init = Set;
        retval = 0;
        break;
    case 2:
        spi_Temp = SPI2;
        SPI2_GPIO_Init(Set);
        Base_SPI2_Init(Width, Set);
        SPI2_Width = Width;
        SPI2_Init = Set;
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
            // kill other
            if(State) {SPI2_NSS_L();}
            else {SPI2_NSS_H();}
        }
        else
        {
            SPI2_NSS_H();
            if(State) {}
            else {}
        }
    #endif
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
    #if SPI_SOFTWARE
    int temp = 0;
    switch (Channel)
    {
    case 0:

        break;
    case 1:
        #if Exist_SPI & OPEN_0010
        for (int i = 0; i < SPI1_Width; i++)
        {
            SPI1_SCK_L();           
            temp = (Data << i) & (0x01 << (SPI1_Width - 1));
            if (temp)
                SPI1_MOSI_H();
            else
                SPI1_MOSI_L();
            SPI1_SCK_H();
        }
        SPI1_MOSI_H();
        #endif
        break;
    case 2:
        #if Exist_SPI & OPEN_0100
        for (int i = 0; i < SPI2_Width; i++)
        {
            SPI2_SCK_L();           // 
            temp = (Data << i) & ((u16)0x01 << (SPI2_Width - 1));
            if (temp)
                SPI2_MOSI_H();      // 
            else
                SPI2_MOSI_L();      // 
            SPI2_SCK_H();           // 
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
    case 1:
        if(SPI1_Init == 0)
        {
            return;
        }
    #if Exist_SPI & OPEN_0010
        spi_Temp = SPI1;
    #endif
        break;
    case 2:
        if(SPI2_Init == 0)
        {
            return;
        }
    #if Exist_SPI & OPEN_0100
        spi_Temp = SPI2;
    #endif
        break;
    default:
        spi_Temp = NULL;
        break;
    }
    if(spi_Temp == NULL)
    {
        return;
    }
    while(SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_TXE) == 0);
    SPI_I2S_ClearFlag(spi_Temp,SPI_I2S_FLAG_TXE);
    SPI_I2S_SendData(spi_Temp, Data);
    while(SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_BSY) == 1);
    SPI_I2S_ClearFlag(spi_Temp,SPI_I2S_FLAG_BSY);
    // 短用这个，长用dma
    #endif

#endif 
}

#if SPI_DMA
    #if Exist_SPI & OPEN_0010
    uint8_t DMA_SPI1_Buff[SPI_DMA_SIZE];
    #endif
    #if Exist_SPI & OPEN_0100
    uint8_t DMA_SPI2_Buff[SPI_DMA_SIZE];
    #endif
#endif

static void Base_SPI_DMA_Send_Data(SPI_mType Channel,const void *Data,int Length)
{
#if SPI_DMA
    DMA_InitTypeDef DMA_InitStructure = {0};
    DMA_Channel_TypeDef *Temp_DMA_Channel = NULL;
    uint32_t DMAy_FLAG;

    uint8_t *p_DMA_BUFF = NULL;
    static char dma_send_First = 0;
	
    switch (Channel)
    {
    case 1:
    #if Exist_SPI & OPEN_0010
        if(SPI1_Init == 0)
        {
            return;
        }
        p_DMA_BUFF = DMA_SPI1_Buff;
        spi_Temp = SPI1;
        DMAy_FLAG = DMA1_FLAG_TC3;
        Temp_DMA_Channel = DMA1_Channel3;
    #endif 
        break;
    case 2:
    #if Exist_SPI & OPEN_0100
        if(SPI2_Init == 0)
        {
            return;
        }
        p_DMA_BUFF = DMA_SPI2_Buff;
        spi_Temp = SPI2;
        DMAy_FLAG = DMA1_FLAG_TC5;
        Temp_DMA_Channel = DMA1_Channel5;
    #endif 
        break;
    default:
        spi_Temp = NULL;
        break;
    }
    // 开始DMA
    if (Data == NULL || p_DMA_BUFF == NULL || (Length <= 0)) {
        return;
    }
    if ((dma_send_First & (0x01 << Channel)) == 0)            //
    {
        dma_send_First |= (0x01 << Channel);
        DMA_ClearFlag(DMAy_FLAG);

        DMA_DeInit(Temp_DMA_Channel);
        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&spi_Temp->DATAR);   /* DATAR: */
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)p_DMA_BUFF;             //
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                  // DMA_DIR_PeripheralSRC(RX)
        DMA_InitStructure.DMA_BufferSize = 0;                          //
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(Temp_DMA_Channel, &DMA_InitStructure);
    }
    else
    {
        while(DMA_GetFlagStatus(DMAy_FLAG) == RESET);   /* Wait until USART TX DMA1 Transfer Complete */
        DMA_ClearFlag(DMAy_FLAG);
    }
    DMA_Cmd(Temp_DMA_Channel, DISABLE);
    memcpy(p_DMA_BUFF,Data,Length);                     //
    DMA_SetCurrDataCounter(Temp_DMA_Channel,Length);
    
    SPI_I2S_DMACmd(spi_Temp,SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(Temp_DMA_Channel, ENABLE);
#endif 
}

void Base_SPI_DMA_Send_Buff(SPI_mType Channel,const void *Data,int Length)
{
    uint8_t *temp_buff = (uint8_t *)Data;
    int temp_num = 0,temp_run = Length;
    while(temp_run > 0)
    {
        temp_num = Length - 0;
        if(temp_num > SPI_DMA_SIZE)
        {
            temp_num = SPI_DMA_SIZE;
        }
        Base_SPI_DMA_Send_Data(Channel,temp_buff,temp_num);
        temp_run -= temp_num;
    }
}

void Base_SPI_ASK_Receive(SPI_mType Channel,uint16_t Data,uint16_t *Receive)
{
#ifdef Exist_SPI
    
#endif 
}

