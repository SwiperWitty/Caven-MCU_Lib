#include "SPI.h"

void SPI_GPIO_Init(int SET)
{
#ifdef Exist_SPI
    GPIO_InitTypeDef  GPIO_InitStructure;
    if (SET)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);   //使能A端口时钟
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
        GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //推挽输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
        GPIO_Init(GPIOB, &GPIO_InitStructure);    //初始化GPIOB

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOA, &GPIO_InitStructure);    //初始化GPIOA
    }
    else
    {
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
        GPIO_Init(GPIOB, &GPIO_InitStructure);    //初始化GPIOB

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
        GPIO_Init(GPIOA, &GPIO_InitStructure);    //初始化GPIOA
    }
#endif
}

//void LCD_GPIO_Init(int SET)
//{
//  GPIO_InitTypeDef  GPIO_InitStructure;
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);   //使能A端口时钟
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
//  if (SET)
//  {
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
//      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         //推挽输出
//      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
//      GPIO_Init(GPIOB, &GPIO_InitStructure);    //初始化GPIOB
//
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//      GPIO_Init(GPIOA, &GPIO_InitStructure);    //初始化GPIOA
//  }
//  else
//  {
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
//      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//      GPIO_Init(GPIOB, &GPIO_InitStructure);    //初始化GPIOB
//
//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//      GPIO_Init(GPIOA, &GPIO_InitStructure);    //初始化GPIOA
//  }
//
//}

char SPIs_Write_And_Read_Byte(char Byte_8,int Speed)					//Master
{
	char read_Byte_8a = 0;
#ifdef Exist_SPI
	char n, m = 0x80;
	SPI_MOSI_H();
	SPI_SCLK_H();
	SPI_CS_L();
	Delay_10us(Speed);
	for (n = 0; n < 8; n++)
	{
		SPI_SCLK_L();
		if (((Byte_8 << n) & m) == m)
			SPI_MOSI_H();
		else
			SPI_MOSI_L();

		Delay_10us(Speed);

		SPI_SCLK_H();
		if (SPI_MISO_IN())												//开始读
			read_Byte_8a |= (m >> n);
		else
			read_Byte_8a |= (0 >> n);
		Delay_10us(Speed);
	}
	SPI_MOSI_H();
	SPI_SCLK_H();
	SPI_CS_H();
#endif
	return read_Byte_8a;
}

void SPI_Software_Init(int SET)
{
#ifdef Exist_SPI
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	if(SET)
	{
		SPI_GPIO_Init();
		SPI_MOSI_H();
		SPI_SCLK_H();
		SPI_CS_H();
	}
	else
	{
		SPI_GPIO_Exit();
	}
#endif
}
