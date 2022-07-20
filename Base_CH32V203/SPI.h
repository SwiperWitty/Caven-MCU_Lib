#ifndef _SPI__H_
#define _SPI__H_

#include "ch32v20x.h"
#include "time.h"

/*
    以 Xxx_Software_Init 作为初始化的库都是软件模拟对应功能（IO可改）
	依赖	"sys_time.h"
    尽量使 SPI_GPIO 在同一组GPIO上（A/B/C/D），这样就无需更改 SPI_RCC_GPIO_TIM 等等
																					2022.02.26
    底层
	对于	GPIO_Speed_50MHz 可不能写 0，不然会报 /enumerated type mixed with another type/
																					2022.04.06	优化
*/

//#define SPI_MISO        GPIO_Pin_14
//#define SPI_MOSI        GPIO_Pin_15
//#define SPI_SCLK		GPIO_Pin_13
//#define SPI_CS  		GPIO_Pin_12

//移植修改

//#define SPI_SCLK_H() GPIOB->BSRR = SPI_SCLK		//置高电平
//#define SPI_SCLK_L() GPIOB->BRR = SPI_SCLK 		//置低电平
//#define SPI_MOSI_H() GPIOB->BSRR = SPI_MOSI
//#define SPI_MOSI_L() GPIOB->BRR = SPI_MOSI
//#define SPI_CS_H() GPIOB->BSRR = SPI_CS
//#define SPI_CS_L() GPIOB->BRR = SPI_CS
//
//#define SPI_MISO_IN() GPIOB->IDR &SPI_MISO 		//读取引脚电平

struct SPI_
{
	char (*Soft_Write_And_Read_Byte) (char Byte_8,int Speed);
	char (*Auto_Write_And_Read_Byte) (char Byte_8,int Speed);
};


char SPIs_Write_And_Read_Byte(char Byte_8,int Speed);	//Master
void SPI_Software_Init(int SET);

#endif
