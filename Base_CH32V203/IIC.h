#ifndef _IIC__H_
#define _IIC__H_

#include "Items.h"

//

#define IIC_Base_Speed  12
#define IIC_Mode_IN     GPIO_Mode_IPU
#define IIC_Mode_OUT    GPIO_Mode_Out_PP


//SPI1

#define IIC_SCL        GPIO_Pin_10
#define IIC_SDA        GPIO_Pin_11
#define GPIO_IIC       GPIOB

#define IIC_SDA_H() GPIO_IIC->BSHR = IIC_SDA        //置高电平
#define IIC_SDA_L() GPIO_IIC->BCR = IIC_SDA         //置低电平
#define IIC_SCL_H() GPIO_IIC->BSHR = IIC_SCL
#define IIC_SCL_L() GPIO_IIC->BCR = IIC_SCL

#define IIC_SDA_IN() GPIO_IIC->INDR & IIC_SDA      //读取引脚电平

//

void IIC_Init(int SET);
void IIC_Write_Addr(char WR,char Addr,int speed);
void IIC_Write_DATA(char DATA,int speed);

#endif
