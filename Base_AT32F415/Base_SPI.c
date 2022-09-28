#include "Base_SPI.h"

#define SPI_Tx_DMA_Channel     DMA1_Channel5

uint8_t SPI_DMA_RX_Buffer[SPI_BufferSize];

int SPI_State[3] = {0};

void SPI1_GPIO_Init(int Set)
{
    GPIO_InitType  GPIO_InitStructure;
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA,ENABLE);
        
        GPIO_InitStructure.GPIO_Pins = SPI1_SCK | SPI1_MOSI;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_OUT;                //时钟、数据输出模式
        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;     //高速
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pins = SPI1_NSS;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_NSS;                //NSS-输出模式（输出）
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pins = SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);

    }
    else
    {
        GPIO_InitStructure.GPIO_Pins = SPI1_NSS|SPI1_SCK|SPI1_MOSI|SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;        //模拟输入
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
    }
}

void SPI2_GPIO_Init(int Set)
{
    GPIO_InitType  GPIO_InitStructure;
    if (Set)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB,ENABLE);
        
        GPIO_InitStructure.GPIO_Pins = SPI2_SCK | SPI2_MOSI;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_OUT;
        GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pins = SPI2_NSS;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_NSS;            //输出模式（输出）
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pins = SPI2_MISO;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);

    }
    else
    {
        GPIO_InitStructure.GPIO_Pins = SPI2_NSS|SPI2_SCK|SPI2_MOSI|SPI2_MISO;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;        //模拟输入
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);
    }
}

void SPI2_DMA_Config (uint8_t *DMA_TX_Buffer,int BufferSize)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1,ENABLE);
    DMA_InitType  DMA_InitStructure;
    NVIC_InitType NVIC_InitStructure;

    DMA_Reset(SPI_Tx_DMA_Channel);
    DMA_DefaultInitParaConfig(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DT;     //外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_TX_Buffer; //内存地址
    DMA_InitStructure.DMA_BufferSize = BufferSize;      //长度
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;    //传输方向，从内存到外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;    //外设地址不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE; //内存地址每次加1
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;   //字节传输增量
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE;   //字节传输宽度
    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;           //非循环模式
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_MEDIUM;   //设置优先级--中
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;      //不是内存到内存
    DMA_Init(SPI_Tx_DMA_Channel, &DMA_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    SPI_I2S_DMAEnable(SPI2,SPI_I2S_DMA_TX, ENABLE);
}

void SPI_Start_Init(int Set)
{
#ifdef Exist_SPI
    FunctionalState set = DISABLE;
    if (Set)
        set = ENABLE;
    #if (SPI_X == 1)
        SPI1_GPIO_Init(Set);
    #elif (SPI_X == 2)
        SPI2_GPIO_Init(Set);
    #endif
    
    #ifndef SPI_Software
        #if (SPI_X == 1)
        SPI_InitType SPI_InitStructure = {0};
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_SPI1, ENABLE);
        
            #ifdef HOST_MODE
            SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
            #else
            SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
            #endif
        SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
        SPI_InitStructure.SPI_FrameSize = SPI_Size;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;          //空闲时SCK高电平
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;        //偶数边沿采样
        SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;   //SPI_NSS_Hard/SPI_NSS_Soft
        SPI_InitStructure.SPI_MCLKP = SPI_Speed;  //分频
        SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;  //高位先行
        SPI_InitStructure.SPI_CPOLY = 7;
        SPI_Init(SPI1, &SPI_InitStructure);

//      SPI_SSOutputCmd(SPI1,ENABLE);

        SPI_Enable(SPI1,set);
        #endif
        #if (SPI_X == 2)
        SPI_InitType SPI_InitStructure = {0};
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_SPI2, ENABLE);
        
            #ifdef HOST_MODE
            SPI_InitStructure.SPI_Mode = SPI_MODE_MASTER;
            #else
            SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
            #endif
        SPI_InitStructure.SPI_TransMode = SPI_TRANSMODE_FULLDUPLEX;
        SPI_InitStructure.SPI_FrameSize = SPI_Size;
        SPI_InitStructure.SPI_CPOL = SPI_CPOL_HIGH;         //空闲时SCK高电平
        SPI_InitStructure.SPI_CPHA = SPI_CPHA_2EDGE;        //偶数边沿采样
        SPI_InitStructure.SPI_NSSSEL = SPI_NSSSEL_SOFT;     //SPI_NSS_Hard/SPI_NSS_Soft
        SPI_InitStructure.SPI_MCLKP = SPI_Speed;  //分频
        SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;  //高位先行
        SPI_InitStructure.SPI_CPOLY = 7;
        SPI_Init(SPI2, &SPI_InitStructure);
//        SPI_NSSHardwareOutputEnable(SPI2,ENABLE);
        #ifdef SPI_DMA
        
        SPI2_DMA_Config (SPI_DMA_RX_Buffer,SPI_BufferSize);     //固定内存和长度（长度随时可变）
        DMA_INTConfig(SPI_Tx_DMA_Channel,DMA_INT_TC,ENABLE);

        #endif
        SPI_Enable(SPI2,set);

        #endif

    #endif
#endif
}

#ifdef SPI_DMA
void DMA1_Channel5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC5) == SET)   //DMA发完了
    {
        DMA_ChannelEnable(SPI_Tx_DMA_Channel,DISABLE);          //停止DMA_5
        SPI_CS_Set(1,DISABLE);                                  //取消片选
        DMA_ClearITPendingBit(DMA1_FLAG_TC5);
    }
}
#endif

#ifdef Exist_SPI
static void SPI_Delay (int time)
{
    #if MCU_SYS_Freq >= 72000000 
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = 10;            //SET
        while((temp--) > 0);
    }
    #else
    while((time--) > 0);
    #endif
}
#endif

void SPI_CS_Set(char Serial,char State)
{
    switch (Serial)
    {
        case 1:
            if (State) {
                SPI_NSS_L();      //  Low
            }
            else {
                SPI_NSS_H();     //  High
            }
            break;
        case 2:

            break;
        default:
            break;
    }

}

void SPI2_DMA_TX(int num)
{

    while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BUSY) != RESET);      //spi不忙
    SPI_CS_Set(1,ENABLE);
    DMA_ChannelEnable(SPI_Tx_DMA_Channel,DISABLE);
    DMA_ClearFlag(DMA1_FLAG_TC5);
    DMA_SetCurrDataCounter(SPI_Tx_DMA_Channel, num);        //改变 SPI_Tx_DMA_Channel ，长度
    DMA_ChannelEnable(SPI_Tx_DMA_Channel,ENABLE);
    // 取消片选在中断 DMA1_Channel5_IRQn
}

void SPI_Send_DATA(const uint16_t DATA)
{
    /*
     * 写标志位
     * 写数据
     * 等忙位
     */
#ifdef HOST_MODE
    SPI_Type *Temp_SPI;
    #if (SPI_X == 1)
    Temp_SPI = SPI1;
    #elif (SPI_X == 2)
    Temp_SPI = SPI2;
    #endif

    #ifdef SPI_Software
    char temp;
    for (int i = 0; i < 8; i++)
    {
        SPI_SCK_H();           //预备上升沿
        temp = (DATA << i) & 0x80;
        if (temp)
            SPI_MOSI_H();      //数据1
        else
            SPI_MOSI_L();      //数据0
        SPI_SCK_L();           //完成上升沿
    }
    SPI_MOSI_H();
    SPI_SCK_H();
    #else
    while(SPI_I2S_GetFlagStatus(Temp_SPI,SPI_I2S_FLAG_TE) == RESET); //检查发送是否完成，完成以后再发送数据
    SPI_I2S_TxData(Temp_SPI, DATA);
    while(SPI_I2S_GetFlagStatus(Temp_SPI,SPI_I2S_FLAG_BUSY) != RESET); //必要  <????>
    #endif
	
#endif
}


//    调用层      //
void SPI_SET_Addr_SendData(char Serial,uint16_t Addr,uint16_t DATA)
{
// Addr &= 0xBFFF;
    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）
    SPI_Delay (1);

    SPI_Send_DATA(Addr);
//    SPI_Delay (1);
    SPI_Send_DATA(DATA);

    SPI_Delay (1);
    SPI_CS_Set(Serial,DISABLE);     //SPI结束
}

uint16_t SPI_SET_Addr_ReadData(char Serial,uint16_t Addr)
{
    /*
     * 等能读
     * 读
     */
    uint16_t temp = 0;
    // Addr &= 0xBFFF;

#ifdef SPI_Software

#else
    SPI_Type *Temp_SPI;
    #if (SPIX == 1)
    Temp_SPI = SPI1;
    #elif (SPIX == 1)
    Temp_SPI = SPI1;
    #endif

    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）
    SPI_Delay (1);
    
    SPI_Send_DATA(Addr);

    SPI_I2S_RxData(Temp_SPI);
    SPI_Send_DATA(0);
    while(SPI_I2S_GetFlagStatus(Temp_SPI,SPI_I2S_FLAG_RNE) == RESET);
    temp = SPI_I2S_RxData(Temp_SPI);

    SPI_Send_DATA(0);
    while(SPI_I2S_GetFlagStatus(Temp_SPI,SPI_I2S_FLAG_RNE) == RESET);
    temp = SPI_I2S_RxData(Temp_SPI);

    SPI_Delay (1);
    SPI_CS_Set(Serial,DISABLE);     //SPI结束
#endif
    return temp;
}

