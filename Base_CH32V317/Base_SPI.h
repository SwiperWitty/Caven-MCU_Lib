#ifndef _BASE_SPI_H__
#define _BASE_SPI_H__

#ifdef DEFAULT
#include "Items.h"              /*	默认功能	*/
#else
#include "User_items.h"         /*	自行设置功能，一般出现在本地文件的User中	*/
#endif

#include "Caven_Type.h"

/****************/

/*
    分为硬件SPI/软件SPI
    DMA和普通模式是可以一起用的
    SPI做主机,如果有多个设备，片选逻辑是：使用a就一直拉低a，直到使用b时，再拉高a片选去拉低b
                                                                                    2026.4.4
*/

typedef enum
{
    m_SPI_CH0 = 0,
    m_SPI_CH1 = 1,
    m_SPI_CH2,
    m_SPI_CH3,
}SPI_mType;

// 选择输出模式
#if Exist_SPI
    #define SPI_DMA_SIZE    BUFF_MAX
    #define HOST_MODE   1
    #define SPI_SOFTWARE    0
    #define SPI_SPEED   SPI_BaudRatePrescaler_8     // 16-4.5MHZ   8-9MHZ     4-18MHZ     2-36MHZ
    #define SPI_MODE_CFG_0   0
    #define SPI_MODE_CFG_3   3

    #ifndef SPI_MODE_CFG
        #define SPI_MODE_CFG     SPI_MODE_CFG_3
    #endif
    #if (SPI_MODE_CFG == SPI_MODE_CFG_0)
        #define SPI_SCLK_IDLE   0
    #elif (SPI_MODE_CFG == SPI_MODE_CFG_3)
        #define SPI_SCLK_IDLE   1
    #else
        #error "SPI_MODE_CFG only support MODE0 or MODE3"
    #endif
    #if SPI_SOFTWARE
        #define SPI_MODE_IN    GPIO_Mode_IPU
        #define SPI_MODE_OUT   GPIO_Mode_Out_PP
    #else
    #define SPI_MODE_IN     GPIO_Mode_IPU
    #define SPI_MODE_OUT    GPIO_Mode_AF_PP
    #define SPI_DMA 1
    #endif
#endif

//SPI1
#define SPI1_NSS        GPIO_Pin_4     //(CS)
#define SPI1_SCK        GPIO_Pin_5
#define SPI1_MISO       GPIO_Pin_6
#define SPI1_MOSI       GPIO_Pin_7
#define GPIO_SPI1       GPIOA

//SPI2
#define SPI2_NSS        GPIO_Pin_12    //(CS)
#define SPI2_SCK        GPIO_Pin_13
#define SPI2_MISO       GPIO_Pin_14
#define SPI2_MOSI       GPIO_Pin_15
#define GPIO_SPI2       GPIOB

/*    驱动层      */

int Base_SPI_Init(SPI_mType Channel, uint8_t Width, int Set, D_pFun cs_pFun);

void Base_SPI_CS_Set(SPI_mType Channel,char Serial,char State);

void Base_SPI_ASK_Receive(SPI_mType Channel,uint16_t Data,uint8_t *Receive,int Length);
void Base_SPI_Send_Data(SPI_mType Channel,uint16_t Data);
void Base_SPI_DMA_Send_Buff(SPI_mType Channel,const void *Data,int Length);


/*******************/
#if Exist_SPI & OPEN_0010
#define SPI1_NSS_H()  GPIO_SPI1->IO_H_REG = SPI1_NSS     // 置高电平
#define SPI1_NSS_L()  GPIO_SPI1->IO_L_REG = SPI1_NSS     // 置低电平
#define SPI1_SCK_H()  GPIO_SPI1->IO_H_REG = SPI1_SCK
#define SPI1_SCK_L()  GPIO_SPI1->IO_L_REG = SPI1_SCK 
#define SPI1_MOSI_H() GPIO_SPI1->IO_H_REG = SPI1_MOSI
#define SPI1_MOSI_L() GPIO_SPI1->IO_L_REG = SPI1_MOSI

#define SPI1_MISO_R() GPIO_ReadInputDataBit(GPIO_SPI1,SPI1_MISO)  // 读取引脚电平
#endif

#if Exist_SPI & OPEN_0100
#define SPI2_NSS_H()  GPIO_SPI2->IO_H_REG = SPI2_NSS     // 置高电平
#define SPI2_NSS_L()  GPIO_SPI2->IO_L_REG = SPI2_NSS     // 置低电平
#define SPI2_SCK_H()  GPIO_SPI2->IO_H_REG = SPI2_SCK
#define SPI2_SCK_L()  GPIO_SPI2->IO_L_REG = SPI2_SCK 
#define SPI2_MOSI_H() GPIO_SPI2->IO_H_REG = SPI2_MOSI
#define SPI2_MOSI_L() GPIO_SPI2->IO_L_REG = SPI2_MOSI

#define SPI2_MISO_R() GPIO_ReadInputDataBit(GPIO_SPI2,SPI2_MISO)  // 读取引脚电平
#endif

#endif
