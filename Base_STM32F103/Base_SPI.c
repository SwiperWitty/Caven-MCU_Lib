#include "Base_SPI.h"

#if Exist_SPI

SPI_TypeDef *spi_Temp;
char SPI1_Width = 0;
char SPI2_Width = 0;
    #if Exist_SPI & OPEN_0010
	uint8_t DMA_SPI1_Buff[500];
	#endif
    #if Exist_SPI & OPEN_0100
	uint8_t DMA_SPI2_Buff[500];
	#endif

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
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = SPI_MODE_OUT;
        GPIO_Init(GPIO_SPI1, &gpio_init_struct);
        
        gpio_init_struct.GPIO_Pin = SPI1_NSS;
        gpio_init_struct.GPIO_Mode = SPI_MODE_NSS;      // NSS-输出模式（输出）
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
        gpio_init(GPIO_SPI1, &gpio_init_struct);
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
        gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
        gpio_init_struct.GPIO_Mode = SPI_MODE_OUT;
        GPIO_Init(GPIO_SPI2, &gpio_init_struct);
        
        gpio_init_struct.GPIO_Pin = SPI2_NSS;
        gpio_init_struct.GPIO_Mode = SPI_MODE_NSS;      // NSS-输出模式（输出）
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
#if Exist_SPI & OPEN_0010
    FunctionalState set = DISABLE;
    SPI_InitTypeDef spi_init_struct;

    spi_Temp = SPI1;
    if (Set)
        set = ENABLE;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

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
    spi_init_struct.SPI_CPHA = SPI_CPHA_1Edge;          // 上升沿让从机读                
    spi_init_struct.SPI_NSS = SPI_NSS_Soft;             // 软件cs
    spi_init_struct.SPI_BaudRatePrescaler = SPI_SPEED;  // 速度
    spi_init_struct.SPI_CRCPolynomial = 7;
    spi_init_struct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    // SPI_Direction_1Line_Tx
    SPI_Init(spi_Temp, &spi_init_struct);
    
    #ifdef SPI_DMA
	DMA_InitTypeDef DMA_InitStruct;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 配置DMA1通道3(SPI1_TX)
    DMA_DeInit(DMA1_Channel3);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(spi_Temp->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)DMA_SPI1_Buff;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = sizeof(DMA_SPI1_Buff);
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3, &DMA_InitStruct);
    DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
    // 配置DMA中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    #endif
    SPI_Cmd(spi_Temp, set);
#endif
}

void Base_SPI2_Init (uint8_t Width,int Set)
{
#if Exist_SPI & OPEN_0100
    FunctionalState set = DISABLE;
    SPI_InitTypeDef spi_init_struct;
    
    spi_Temp = SPI2;
    if (Set)
        set = ENABLE;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

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
    spi_init_struct.SPI_CPHA = SPI_CPHA_1Edge;          // 上升沿让从机读                
    spi_init_struct.SPI_NSS = SPI_NSS_Soft;             // 软件cs
    spi_init_struct.SPI_BaudRatePrescaler = SPI_SPEED;  // 速度
    spi_init_struct.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_Init(spi_Temp, &spi_init_struct);
    
    #ifdef SPI_DMA
	DMA_InitTypeDef DMA_InitStruct;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 配置DMA1通道5(SPI2_TX)
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(spi_Temp->DR);
    DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)DMA_SPI2_Buff;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStruct.DMA_BufferSize = sizeof(DMA_SPI2_Buff);
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStruct);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
    // 配置DMA中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    #endif
    SPI_Cmd(spi_Temp, set);
#endif
}
#endif

int Base_SPI_Init(SPI_mType Channel,uint8_t Width,int Set)
{
    int retval = -1;
#if Exist_SPI

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
#if Exist_SPI
    
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
#if Exist_SPI
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
    // 等待发送缓冲区空
    while(SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_TXE) == RESET);
    // 发送字节
    SPI_I2S_SendData(spi_Temp, Data);
    // 等待发送完成
    while(SPI_I2S_GetFlagStatus(spi_Temp, SPI_I2S_FLAG_BSY) == SET);
	
    #endif

#endif 
}

#ifdef SPI_DMA

    #if Exist_SPI & OPEN_0010
volatile char SPI1_DMA_Flag = 0;
void SPI1_FINISH_HANDLERIT()
{
    if(DMA_GetITStatus(DMA1_IT_TC3))       // 传输完成
    {
		Base_SPI_CS_Set(m_SPI_CH1,1,0);
		SPI1_DMA_Flag = 0;
        DMA_ClearITPendingBit(DMA1_IT_TC3);
    }
}
    #endif 
    #if Exist_SPI & OPEN_0100
volatile char SPI2_DMA_Flag = 0;
void SPI2_FINISH_HANDLERIT()
{
    if(DMA_GetITStatus(DMA1_IT_TC4))       // 传输完成
    {
        DMA_ClearITPendingBit(DMA1_IT_TC4);
    }
    if(DMA_GetITStatus(DMA1_IT_TC5))       // 传输完成
    {
		Base_SPI_CS_Set(m_SPI_CH2,1,0);
		SPI2_DMA_Flag = 0;
        DMA_ClearITPendingBit(DMA1_IT_TC5);
    }
}
    #endif 
#endif 



void Base_SPI_DMA_Send_Data(SPI_mType Channel,const void *Data_array,int size)
{
#ifdef SPI_DMA
    DMA_Channel_TypeDef *Temp_DMA_Channel;
    
//    uint32_t DMAy_FLAG;
    uint8_t *p_DMA_BUFF = NULL;
    volatile char *SPI_DMA_Flag_Temp = 0;
	
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

    #endif 
        break;
    case 2:
    #if Exist_SPI & OPEN_0100
        spi_Temp = SPI2;
        p_DMA_BUFF = DMA_SPI2_Buff;
        SPI_DMA_Flag_Temp = &SPI2_DMA_Flag;
        Temp_DMA_Channel = DMA1_Channel5;

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

    Base_SPI_CS_Set(Channel,1,1);
    
    DMA_SetCurrDataCounter(Temp_DMA_Channel, size);
    DMA_Cmd(Temp_DMA_Channel, ENABLE);
    SPI_I2S_DMACmd(spi_Temp, SPI_I2S_DMAReq_Tx, ENABLE);     /* begin dma transmitting */
#endif 
}

void Base_SPI_ASK_Receive(SPI_mType Channel,uint16_t Data,uint16_t *Receive)
{
#if Exist_SPI
    
#endif 
}

