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
        GPIO_InitStructure.GPIO_Pin = SPI2_SCK|SPI2_MOSI | SPI2_NSS;
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
    #ifndef SPI_Software
        SPI_InitTypeDef SPI_InitStructure = {0};
        NVIC_InitTypeDef NVIC_InitStructure = {0};
		int speed_baud = SPI_Speed;
		if (speed_baud > 0) {
			speed_baud -= 8;                            //可以超频就超频（与SPI1保持同速）
		}
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
            SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;         //空闲时SCK高电平
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        //偶数边沿采样
            SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;   //SPI_NSS_Hard/SPI_NSS_Soft
            SPI_InitStructure.SPI_BaudRatePrescaler = SPI_Speed;  //分频
            SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //高位先行
            SPI_InitStructure.SPI_CRCPolynomial = 7;
            SPI_Init(SPI1, &SPI_InitStructure );

            NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
            NVIC_Init(&NVIC_InitStructure);
            SPI_SSOutputCmd(SPI1,ENABLE);
            SPI_Cmd(SPI1,ENABLE);
//          SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_TXE,ENABLE);
    #endif
            break;
        case 2:
            SPI2_GPIO_Init(SET);
    #ifndef SPI_Software
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );   //开始硬件SPI2,API1
        #if (SPI_MODE == HOST_MODE)
            SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
        #elif (SPI_MODE == SLAVE_MODE)
            SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
        #endif
            SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
            SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
            SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;         //空闲时SCK高电平
            SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;        //偶数边沿采样
            SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;   //SPI_NSS_Hard/SPI_NSS_Soft
            SPI_InitStructure.SPI_BaudRatePrescaler = speed_baud;  //分频
            SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //高位先行
            SPI_InitStructure.SPI_CRCPolynomial = 7;
            SPI_Init(SPI2, &SPI_InitStructure );

            NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
            NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
            NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
            NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//                NVIC_Init(&NVIC_InitStructure);
            SPI_SSOutputCmd(SPI2, ENABLE );
            SPI_Cmd(SPI2,ENABLE);

            break;
        default:
            break;
    #endif
    }
#endif
}

void SPI_Send_DATA(char Channel,const char DATA)
{
#ifdef Exist_SPI
    #if(SPI_MODE == HOST_MODE)

    switch (Channel) {
        case 1:
        #ifdef SPI_Software
            SPI1_NSS_L();               //SPI开始（片选）
            for (int i = 0; i < 8; i++)
            {
                SPI1_SCK_L();           //预备上升沿
                if ((DATA << i) & 0x80)
                    SPI1_MOSI_H();      //数据1
                else
                    SPI1_MOSI_L();      //数据0
                SPI1_SCK_H();           //完成上升沿
            }
            SPI1_NSS_H();               //SPI结束
            SPI1_MOSI_H();
        #else
            while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET); //检查发送是否完成，完成以后再发送数据
            SPI_I2S_SendData(SPI1, DATA);
            while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_BSY) != RESET); //必要

        #endif
            break;

        case 2:
        #ifdef SPI_Software
            SPI2_NSS_L();               //SPI开始（片选）
            for (int i = 0; i < 8; i++)
            {
                SPI2_SCK_L();           //预备上升沿
                if ((DATA << i) & 0x80)
                    SPI2_MOSI_H();      //数据1
                else
                    SPI2_MOSI_L();      //数据0
                SPI2_SCK_H();           //完成上升沿
            }
            SPI2_NSS_H();               //SPI结束
            SPI2_MOSI_H();
        #else
            while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET); //检查发送是否完成，完成以后再发送数据
            SPI_I2S_SendData(SPI2, DATA);
            while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY) != RESET); //必要
        #endif
            break;
        default:
            break;
    }
    #endif
#endif
}

