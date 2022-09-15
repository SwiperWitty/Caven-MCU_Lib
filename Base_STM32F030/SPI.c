#include "SPI.h"

int SPI_State[3] = {0};

void SPI1_GPIO_Init(int SET)
{
#if (SPI_X == 1)
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = SPI1_SCK | SPI1_MOSI;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_OUT;        //输出模式（备用）
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //PP推挽    OD开漏（有阻）
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //上下拉
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //高速
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = SPI1_NSS;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       //输出模式（输出）
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);

    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = SPI1_NSS|SPI1_SCK|SPI1_MOSI|SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;        //模拟输入
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    //上下拉
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
    }
#endif
}

void SPIx_Init(char Channel,int SET)
{
#ifdef Exist_SPI
    SPI_State[Channel] = SET;
    #ifndef SPI_Software
        SPI_InitTypeDef SPI_InitStructure = {0};
        NVIC_InitTypeDef NVIC_InitStructure = {0};
    #endif
    switch (Channel)
    {
        case 1:
            SPI1_GPIO_Init(SET);
    #ifndef SPI_Software                                    //开始硬件SPI
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE );
        #if (SPI_MODE == HOST_MODE)
            SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
        #elif (SPI_MODE == SLAVE_MODE)
            SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
        #endif
            SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
            SPI_InitStructure.SPI_DataSize = SPI_Size;
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;          //空闲时SCK高电平
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;        //偶数边沿采样
            SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard/SPI_NSS_Soft
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_Speed;  //分频
            SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //高位先行
            SPI_InitStructure.SPI_CRCPolynomial = 7;
            SPI_Init(SPI1, &SPI_InitStructure);

            NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
//            SPI_SSOutputCmd(SPI1,ENABLE);

            SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
            SPI_Cmd(SPI1,ENABLE);
//          SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,ENABLE);
            
    #endif
            break;
        case 2:
            break;
        default:
            break;
    
    }
#endif
}

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

void SPI_CS_Set(char Serial,char State)
{
#if (SPI_X == 1)
    switch (Serial)
    {
        case 1:
            if (State) {
                GPIOA->BRR = SPI1_NSS;      //  Low
            }
            else {
                GPIOA->BSRR = SPI1_NSS;     //  High
            }
            break;
        case 2:

            break;
        default:
            break;
    }
#endif
#if (SPI_X == 2)

#endif

}

void SPI1_Send_DATA(char Serial,const uint16_t DATA)
{
    /*
     * 写标志位
     * 写数据
     * 等忙位
     */
#if (SPI_X == 1)
#if(SPI_MODE == HOST_MODE)

    #ifdef SPI_Software
    char temp;
    for (int i = 0; i < 8; i++)
    {
        SPI1_SCK_H();           //预备上升沿
        temp = (DATA << i) & 0x80;
        if (temp)
            SPI1_MOSI_H();      //数据1
        else
            SPI1_MOSI_L();      //数据0
        SPI1_SCK_L();           //完成上升沿
    }
    SPI1_MOSI_H();
    SPI1_SCK_H();
    #else
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET); //检查发送是否完成，完成以后再发送数据
    SPI_SendData8(SPI1, DATA);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) != RESET); //必要
    #endif

#endif
#endif
}

void SPI2_Send_DATA(char Serial,const uint16_t DATA)
{
#if (SPI_X == 2)

#endif
}


void SPI_SET_Addr_SendData(char Serial,uint16_t Addr,uint16_t DATA)
{
    Addr &= 0xBFFF;

#if (SPI_X == 1)
    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）
    SPI_Delay (1);
    SPI1_Send_DATA(Serial,Addr);
//    SPI_Delay (1);
    SPI1_Send_DATA(Serial,DATA);
    SPI_Delay (1);
    SPI_CS_Set(Serial,DISABLE);     //SPI结束
#elif (SPI_X == 2)


#endif
}

uint16_t SPI_SET_Addr_ReadData(char Serial,uint16_t Addr)
{
    /*
     * 等能读
     * 读
     *
     */
    uint16_t temp = 0;
    Addr &= 0xBFFF;
#ifndef SPI_Software
    #if (SPI_X == 1)
    SPI_CS_Set(Serial,ENABLE);      //SPI开始（片选）
    SPI_Delay (1);
    SPI1_Send_DATA(Serial,Addr | 0x8000);

    SPI1_Send_DATA(Serial,0);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
    temp = SPI_ReceiveData8(SPI1);

    SPI1_Send_DATA(Serial,0);
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
    temp = SPI_ReceiveData8(SPI1);
    SPI_Delay (1);
    SPI_CS_Set(Serial,DISABLE);     //SPI结束
    #elif (SPI_X == 2)
    SPI2_Send_DATA(Serial,Addr);
    SPI2_Send_DATA(Serial,0);
    while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) == RESET);
    temp = SPI_I2S_ReceiveData(SPI2);
    #endif
#endif


    return temp;
}

