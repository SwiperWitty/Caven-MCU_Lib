#include "SPI.h"

int SPI_State[3] = {0};

void SPI1_GPIO_Init(int SET)
{
#ifdef Exist_SPI
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //GPIOA
        GPIO_InitStructure.GPIO_Pin = SPI1_SCK|SPI1_MOSI | SPI1_NSS;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = SPI1_NSS;                 //NSS 不推荐复用
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = SPI1_NSS|SPI1_SCK|SPI1_MOSI|SPI1_MISO;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIO_SPI1, &GPIO_InitStructure);
    }
#endif
}

void SPI2_GPIO_Init(int SET)
{
#ifdef Exist_SPI
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //GPIOB
        GPIO_InitStructure.GPIO_Pin = SPI2_NSS|SPI2_SCK|SPI2_MOSI;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = SPI2_MISO;
        GPIO_InitStructure.GPIO_Mode = SPI_MODE_IN;
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = SPI2_NSS|SPI2_SCK|SPI2_MOSI|SPI2_MISO;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIO_SPI2, &GPIO_InitStructure);
    }
#endif
}

void SPIx_Init(char Channel,int SET)
{
#ifdef Exist_SPI
    SPI_State[Channel] = SET;
    switch (Channel) {
        case 1:
            SPI1_GPIO_Init(SET);
        #ifndef SPI_Software                                    //开始硬件SPI
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE );
            SPI_InitTypeDef SPI_InitStructure = {0};
            NVIC_InitTypeDef NVIC_InitStructure = {0};
            #if (SPI_MODE == HOST_MODE)
                SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

            #elif (SPI_MODE == SLAVE_MODE)
                SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
            #endif
                SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
                SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
                SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;         //空闲时SCK高电平
                SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        //偶数边沿采样
                SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard/SPI_NSS_Soft
                SPI_InitStructure.SPI_BaudRatePrescaler = SPI_Speed;  //分频
                SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //高位先行
                SPI_InitStructure.SPI_CRCPolynomial = 7;
                SPI_Init(SPI1, &SPI_InitStructure );

                NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
                NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
                NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
                NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
                NVIC_Init(&NVIC_InitStructure);

                SPI_Cmd(SPI1,ENABLE);
//                SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,ENABLE);
        #endif
            break;
        case 2:
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
            SPI2_GPIO_Init(SET);
        #ifndef SPI_Software

        #endif
            break;
        default:
            break;
    }
#endif
}

void SPI_Send_DATA(char Channel,char DATA)
{
#ifdef Exist_SPI
    char temp;
    #if(SPI_MODE == HOST_MODE)
    switch (Channel) {
        case 1:
            SPI1_NSS_L();               //SPI开始（片选）
//            SPI_SSOutputCmd(SPI1, ENABLE);
        #ifdef SPI_Software
            SPI1_NSS_L();               //SPI开始（片选）
            SPI1_SCK_L();
            for (int i = 0; i < 8; i++)
            {
                SPI1_SCK_L();           //预备上升沿
                temp = (DATA << i) & 0x80;
                if (temp)
                    SPI1_MOSI_H();      //数据1
                else
                    SPI1_MOSI_L();      //数据0
                SPI1_SCK_H();           //完成上升沿
            }
            SPI1_NSS_H();               //SPI结束
        #else
            while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET); //检查发送是否完成，完成以后再发送数据
            SPI_I2S_SendData(SPI1, DATA);
            while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) != RESET); //必要
        #endif
            SPI1_NSS_H();               //SPI结束
//            SPI_SSOutputCmd(SPI1, DISABLE);
            break;

        case 2:
            SPI2_NSS_L();               //SPI开始（片选）
            SPI2_SCK_L();
            for (int i = 0; i < 8; i++)
            {
                SPI2_SCK_L();           //预备上升沿
                temp = (DATA << i) & 0x80;
                if (temp)
                    SPI2_MOSI_H();      //数据1
                else
                    SPI2_MOSI_L();      //数据0
                SPI2_SCK_H();           //完成上升沿
            }
            SPI2_NSS_H();               //SPI结束
            break;
        default:
            break;
    }
    #endif
#endif
}

#ifndef SPI_Software

#endif
