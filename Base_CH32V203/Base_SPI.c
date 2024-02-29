#include "Base_SPI.h"

#ifdef Exist_SPI

    SPI_TypeDef *spi_Temp;
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
        gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;      // NSS-输出模式（输出）
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
        gpio_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;      // NSS-输出模式（输出）
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
    这个是给硬件SPI的初始化
*/
void Base_SPI1_Init (uint8_t Width,int Set)
{
    FunctionalState set = DISABLE;
    SPI_InitTypeDef spi_init_struct;
    SPI_StructInit(&spi_init_struct);
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    spi_Temp = SPI1;
    if (Set)
        set = ENABLE;
    SPI_I2S_DeInit(spi_Temp);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
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
    spi_init_struct.SPI_CPOL = SPI_CPOL_High;           // 闲时 SCK状态
    spi_init_struct.SPI_CPHA = SPI_CPHA_2Edge;                // 上升沿让从机读
    spi_init_struct.SPI_NSS = SPI_NSS_Soft;           // 软件cs
    spi_init_struct.SPI_BaudRatePrescaler = SPI_SPEED;                     // 速度
    spi_init_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;       // SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    spi_init_struct.SPI_CRCPolynomial = 7;
    SPI_Init(spi_Temp, &spi_init_struct);

    #ifdef SPI_DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    //
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;    // DMA1_Channel5_IRQn
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_DeInit(DMA1_Channel3);
    DMA_ITConfig(DMA1_Channel3,DMA_IT_TC, ENABLE);
    #endif
    SPI_Cmd(spi_Temp, set);
}

void Base_SPI2_Init (uint8_t Width,int Set)
{
    FunctionalState set = DISABLE;
    SPI_InitTypeDef spi_init_struct;
    SPI_StructInit(&spi_init_struct);
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    spi_Temp = SPI2;
    if (Set)
        set = ENABLE;
    SPI_I2S_DeInit(spi_Temp);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
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
    spi_init_struct.SPI_CPOL = SPI_CPOL_High;           // 闲时 SCK状态
    spi_init_struct.SPI_CPHA = SPI_CPHA_2Edge;                // 上升沿让从机读
    spi_init_struct.SPI_NSS = SPI_NSS_Soft;           // 软件cs
    spi_init_struct.SPI_BaudRatePrescaler = SPI_SPEED;                     // 速度
    spi_init_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;       // SPI_TRANSMIT_HALF_DUPLEX_TX SPI_TRANSMIT_FULL_DUPLEX
    spi_init_struct.SPI_CRCPolynomial = 7;
    SPI_Init(spi_Temp, &spi_init_struct);

    #ifdef SPI_DMA
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    //
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;    //
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_DeInit(DMA1_Channel5);
    DMA_ITConfig(DMA1_Channel5,DMA_IT_TC, ENABLE);
    #endif
    SPI_Cmd(spi_Temp, set);
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
    while(SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_TXE) == RESET);   //发送缓冲区空了
    SPI_I2S_SendData(spi_Temp, Data);
    while(SPI_I2S_GetFlagStatus(spi_Temp,SPI_I2S_FLAG_BSY) != RESET);     //SPI忙就会 = 1，不忙就是0

    #endif

#endif
}

#ifdef SPI_DMA

    #if Exist_SPI & OPEN_0010
uint8_t DMA_SPI1_Buff[500];
volatile char SPI1_DMA_Flag = 0;
void SPI1_FINISH_HANDLERIT()
{
    if(DMA_GetITStatus(DMA1_IT_TC3) != RESET)       // 传输完成
    {
        if(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) == RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TC3);
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
    if(DMA_GetITStatus(DMA1_IT_TC5) != RESET)       // 传输完成
    {
        if(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY) == RESET)
        {
            DMA_ClearITPendingBit(DMA1_IT_TC5);
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

    DMA_InitTypeDef dma_init_struct;
    DMA_Channel_TypeDef *Temp_DMA_Channel;

//    uint32_t DMAy_FLAG;
    uint8_t *p_DMA_BUFF = NULL;
    volatile char *SPI_DMA_Flag_Temp = 0;
    DMA_StructInit(&dma_init_struct);
    dma_init_struct.DMA_BufferSize = size;
    dma_init_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma_init_struct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    switch (Channel)
    {
    case 0:

        break;
    case 1:
    #if Exist_SPI & OPEN_0010
        spi_Temp = SPI1;
        p_DMA_BUFF = DMA_SPI1_Buff;
        SPI_DMA_Flag_Temp = &SPI1_DMA_Flag;
        Temp_DMA_Channel = DMA1_Channel3;
        if(SPI1_Width > 8)
        {
            dma_init_struct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
            dma_init_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
            dma_init_struct.DMA_BufferSize = size / 2;
        }
    #endif
        break;
    case 2:
    #if Exist_SPI & OPEN_0100
        spi_Temp = SPI2;
        p_DMA_BUFF = DMA_SPI2_Buff;
        SPI_DMA_Flag_Temp = &SPI2_DMA_Flag;
        Temp_DMA_Channel = DMA1_Channel5;
        if(SPI2_Width > 8)
        {
            dma_init_struct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
            dma_init_struct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
            dma_init_struct.DMA_BufferSize = size / 2;
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
    Base_SPI_CS_Set(Channel,1,ENABLE);

    DMA_Cmd(Temp_DMA_Channel, DISABLE);

    //
    dma_init_struct.DMA_PeripheralBaseAddr = (u32)(&spi_Temp->DATAR);   /* USARTx->DATAR: */
    dma_init_struct.DMA_MemoryBaseAddr = (u32)p_DMA_BUFF;             //
    dma_init_struct.DMA_DIR = DMA_DIR_PeripheralDST;                  // DMA_DIR_PeripheralSRC(RX)
    dma_init_struct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init_struct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init_struct.DMA_Mode = DMA_Mode_Normal;
    dma_init_struct.DMA_Priority = DMA_Priority_VeryHigh;
    dma_init_struct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(Temp_DMA_Channel, &dma_init_struct);

    DMA_Cmd(Temp_DMA_Channel, ENABLE);     /* begin dma transmitting */
    SPI_I2S_DMACmd(spi_Temp,SPI_I2S_DMAReq_Tx, ENABLE);
#endif
}

void Base_SPI_ASK_Receive(SPI_mType Channel,uint16_t Data,uint16_t *Receive)
{
#ifdef Exist_SPI

#endif
}

