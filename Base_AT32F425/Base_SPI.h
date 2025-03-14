#ifndef _BASE_SPI_H__
#define _BASE_SPI_H__

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/****************/

/*
            SCK    ->  
                        /----\
            MISO   ->   |SPI1|    <-    NSS/NSS2/NSS3  (Serial) 
                        \----/
            MOSI   ->
    
    分为硬件SPI/软件SPI
    软件SPI慢，但是它可以指定任意IO口(目前只发不收)。
    尽量使 SPI_GPIO 在同一组GPIO上（A/B/C/D）
    SPI是主动通信（主从），主机的收发逻辑不需要中断，但是从机需要（傻了吧，这是不全面的）。
    DMA和普通模式是可以一起用的
																					2022.02.26
    SPI的唯一要求————越快越好，目前软件模拟 461kHz(8bit)，硬件SPI参考-SPI_Speed-定义。
    软件模式 -- 4Mhz
    硬件普通模式 -- 36Mhz
    硬件DMA模式  -- 36Mhz
                                                                                    2022.07.26
    SPI的软件模拟方式为size-8bit,硬件SPI可以是size-8bit/size-16bit
    SPI软件方模拟方式为上升沿读(0,0)
    SPI一般只会使用一个（SPI1），但是硬件可能被占用于是选择SPI2（推荐只使用一个SPI）
                                                                                    2022.08.15
    SPI的硬件NSS就是一直拉低。狗都不用。
                                                                                    2022.08.19  
    SPI做主机：软件、硬件（包括DMA）完成，没有DMA接收
                                                                                    2022.10.14                                                                                                                                                                

*/

typedef enum
{
//    m_SPI_CH0 = 0,
    m_SPI_CH1 = 1,
    m_SPI_CH2,
//    m_SPI_CH3,
}SPI_mType;

// 选择输出模式
#ifdef Exist_SPI
//    #define SPI_Software	        //屏蔽就是硬件模式
    #ifndef SPI_Software
        #define SPI_DMA			    //屏蔽就是普通模式
    #endif
    #define HOST_MODE
    #define SPIx   2
#endif


#ifdef SPI_Software                                 //软件SPI
#define SPI_MODE_IN    GPIO_MODE_INPUT
#define SPI_MODE_OUT   GPIO_MODE_OUTPUT
#else                                               //硬件SPI
#define SPI_MODE_IN    GPIO_MODE_INPUT
#define SPI_MODE_OUT   GPIO_MODE_MUX
#define SPI_Speed   SPI_MCLK_DIV_8        //16-9MHZ   8-18MHZ     4-36MHZ     2-72MHZ
#define SPI_Size    SPI_FRAME_8BIT                  //8b   16b
#endif
#define SPI_MODE_NSS    GPIO_MODE_OUTPUT

//SPI1
#define SPI1_NSS        GPIO_PINS_4     //(CS)
#define SPI1_SCK        GPIO_PINS_5
#define SPI1_MISO       GPIO_PINS_6
#define SPI1_MOSI       GPIO_PINS_7
#define GPIO_SPI1       GPIOA

//SPI2
#define SPI2_NSS        GPIO_PINS_12    //(CS)
#define SPI2_SCK        GPIO_PINS_13
#define SPI2_MISO       GPIO_PINS_14
#define SPI2_MOSI       GPIO_PINS_15
#define GPIO_SPI2       GPIOB


//    驱动层      //

int Base_SPI_Init(SPI_mType Channel,uint8_t Width,int Set);

void Base_SPI_CS_Set(SPI_mType Channel,char Serial,char State);

void Base_SPI_ASK_Receive(SPI_mType Channel,uint16_t Data,uint16_t *Receive);
void Base_SPI_Send_Data(SPI_mType Channel,uint16_t Data);
void Base_SPI_DMA_Send_Data(SPI_mType Channel,const void *Data_array,int Length);



/*******************/
#if Exist_SPI & OPEN_0010
#define SPI1_NSS_H()  GPIO_SPI1->IO_H_REG = SPI1_NSS     // 置高电平
#define SPI1_NSS_L()  GPIO_SPI1->IO_L_REG = SPI1_NSS     // 置低电平
#define SPI1_SCK_H()  GPIO_SPI1->IO_H_REG = SPI1_SCK
#define SPI1_SCK_L()  GPIO_SPI1->IO_L_REG = SPI1_SCK 
#define SPI1_MOSI_H() GPIO_SPI1->IO_H_REG = SPI1_MOSI
#define SPI1_MOSI_L() GPIO_SPI1->IO_L_REG = SPI1_MOSI

#define SPI1_MISO_R() gpio_input_data_bit_read(GPIO_SPI1,SPI1_MISO)  // 读取引脚电平
#endif

#if Exist_SPI & OPEN_0100
#define SPI2_NSS_H()  GPIO_SPI2->IO_H_REG = SPI2_NSS     // 置高电平
#define SPI2_NSS_L()  GPIO_SPI2->IO_L_REG = SPI2_NSS     // 置低电平
#define SPI2_SCK_H()  GPIO_SPI2->IO_H_REG = SPI2_SCK
#define SPI2_SCK_L()  GPIO_SPI2->IO_L_REG = SPI2_SCK 
#define SPI2_MOSI_H() GPIO_SPI2->IO_H_REG = SPI2_MOSI
#define SPI2_MOSI_L() GPIO_SPI2->IO_L_REG = SPI2_MOSI

#define SPI2_MISO_R() gpio_input_data_bit_read(GPIO_SPI2,SPI2_MISO)  // 读取引脚电平
#endif

#endif
