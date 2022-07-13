#include "base.h"

/*	本系统的索引结构体	*/			//本地boy
struct Base_Init_ Base_Init;
struct Base_User_ Base_User;        //结构体实体


void Base_User_index(void)		//索引 功能函数 本体		（这是强制绑定函数本体）
{

	Base_User.ADC.Read_MCU_Temp = Read_MCU_Temp;
	Base_User.ADC.ADC_x_Read_Vol = ADC_x_Read_Vol;

	Base_User.DACx.DAC_x_Set_Vol = DAC_x_Set_Vol;

}

void Base_Index(void)		//索引 初始化函数 和 功能函数
{
	Base_Init.IIC_Software_Init = IIC_Software_Init;	//其实这里结构体内的函数指针直接绑定函数就可以了
	Base_Init.SPI_Software_Init = SPI_Software_Init;	//这样在这就可以直接追踪到.c文件

	Base_Init.ADC_x_Init = ADC_x_Init;
	Base_Init.DAC_x_Init = DAC_x_Init;
	
	Base_Init.PWM_x_Init = PWM_x_Init;
		
	Base_Init.Flash_Init = Flash_Init;		//这些内容的初始化也有索引
	
	Base_User_index();						//初始化 功能索引
}

