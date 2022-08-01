#ifndef _SPI__H_
#define _SPI__H_

#include "Items.h"

/*
    分为硬件SPI/软件SPI
    软件SPI慢，但是它可以指定任意IO口。
    尽量使 SPI_GPIO 在同一组GPIO上（A/B/C/D），这样就无需更改 SPI_RCC_GPIO_TIM 等等
																					2022.02.26
    SPI的唯一要求————越快越好，目前软件模拟 461kHz(8bit)，硬件SPI参考-SPI_Speed-定义。

                                                                                    2022.07.26
    底层
	对于	GPIO_Speed_50MHz 可不能写 0，不然会报 /enumerated type mixed with another type/
																					2022.04.06	优化
*/

// 选择输出模式

#define SPI_Software

#ifdef SPI_Software
#define SPI_MODE_IN    GPIO_Mode_IPU
#define SPI_MODE_OUT   GPIO_Mode_Out_PP
#else
#define SPI_MODE_IN    GPIO_Mode_IN_FLOATING
#define SPI_MODE_OUT   GPIO_Mode_AF_PP
#endif

#define SPI_Speed   SPI_BaudRatePrescaler_16        //16-9MHZ   8-18MHZ     4-36MHZ     2-72MHZ
//SPI1
#define SPI1_NSS        GPIO_Pin_4      //(CS)
#define SPI1_SCK        GPIO_Pin_5
#define SPI1_MISO       GPIO_Pin_6
#define SPI1_MOSI       GPIO_Pin_7
#define GPIO_SPI1       GPIOA

#define SPI1_SCK_H()  GPIO_SPI1->BSRR = SPI1_SCK		//置高电平
#define SPI1_SCK_L()  GPIO_SPI1->BRR = SPI1_SCK 		//置低电平
#define SPI1_MOSI_H() GPIO_SPI1->BSRR = SPI1_MOSI
#define SPI1_MOSI_L() GPIO_SPI1->BRR = SPI1_MOSI
#define SPI1_NSS_H()  GPIO_SPI1->BSRR = SPI1_NSS
#define SPI1_NSS_L()  GPIO_SPI1->BRR = SPI1_NSS

#define SPI1_MISO_IN() GPIO_SPI1->IDR & SPI1_MISO      //读取引脚电平

//SPI2
#define SPI2_NSS        GPIO_Pin_12      //(CS)
#define SPI2_SCK        GPIO_Pin_13
#define SPI2_MISO       GPIO_Pin_14
#define SPI2_MOSI       GPIO_Pin_15
#define GPIO_SPI2       GPIOB

#define SPI2_SCK_H()  GPIO_SPI2->BSRR = SPI2_SCK        //置高电平
#define SPI2_SCK_L()  GPIO_SPI2->BRR = SPI2_SCK         //置低电平
#define SPI2_MOSI_H() GPIO_SPI2->BSRR = SPI2_MOSI
#define SPI2_MOSI_L() GPIO_SPI2->BRR = SPI2_MOSI
#define SPI2_NSS_H()  GPIO_SPI2->BSRR = SPI2_NSS
#define SPI2_NSS_L()  GPIO_SPI2->BRR = SPI2_NSS

#define SPI2_MISO_IN() GPIO_SPI2->IDR & SPI2_MISO      //读取引脚电平

//
//void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
//void SPI1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void SPIx_Init(char Channel,int SET);
void SPI_Send_DATA(char Channel,const char DATA);

#endif
