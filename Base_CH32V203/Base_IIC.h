#ifndef _IIC__H_
#define _IIC__H_

#if DEFAULT
#include "Items.h"              //默认功能
#else
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/*
 * 标准软件模拟IIC
 * MAX  36kHZ(8bit+ACK)
 * MAX  33kHZ(8bit+WaitASK-YES)
 * MAX  19kHZ(8bit+WaitASK-NO)
 * MAX-Pro  142kHZ(8bit+ACK)
 */

// 配置
#ifdef Exist_IIC
#include "Base_Exist_GPIO.h"

#define IIC_Base_Speed  20

//
#define IIC_CONFIG      0       // L 上升沿读取
#define IIC_SCL         10   // GPIO_Pin_10
#define IIC_SDA         11   // GPIO_Pin_11
#define GPIO_IIC        2

#endif
//

void Base_IIC_Init(int set);
char Base_IIC_Send_DATA(char Addr,const uint8_t *Data,char ACK,int Length,int Speed,char continuous);
char Base_IIC_Receive_DATA(char Addr,uint8_t *Data,char ACK,int Length,int Speed);



#endif
