#ifndef _IIC__H_
#define _IIC__H_

#ifdef DEFAULT
#include "Items.h"              //Ĭ�Ϲ���
#endif

#ifndef DEFAULT
#include "User_items.h"         //�������ù��ܣ�һ������ڱ����ļ���User��
#endif

/****************/

/*
 * ��׼����ģ��IIC
 * MAX  36kHZ(8bit+ACK)
 * MAX  33kHZ(8bit+WaitASK-YES)
 * MAX  19kHZ(8bit+WaitASK-NO)
 * MAX-Pro  142kHZ(8bit+ACK)
 */

//����
#ifdef Exist_IIC
#define IIC_Base_Speed  12
#define IIC_Mode_IN     GPIO_Mode_IN
#define IIC_Mode_OUT    GPIO_Mode_OUT


//IIC_GPIO

#define IIC_SCL        GPIO_Pin_10
#define IIC_SDA        GPIO_Pin_11
#define GPIO_IIC       GPIOB

#define IIC_SDA_H() GPIO_IIC->BSRR = IIC_SDA         //�øߵ�ƽ
#define IIC_SDA_L() GPIO_IIC->BRR = IIC_SDA         //�õ͵�ƽ
#define IIC_SCL_H() GPIO_IIC->BSRR = IIC_SCL
#define IIC_SCL_L() GPIO_IIC->BRR = IIC_SCL

#define IIC_SDA_R() (GPIO_IIC->IDR & IIC_SDA)         //��ȡ���ŵ�ƽ
#endif
//

void IIC_Start_Init(int Set);
char IIC_Send_DATA(char Addr,const char *Data,char ACK,int Length,int Speed);
char IIC_Receive_DATA(char Addr,char *Target,char ACK,int Length,int Speed);



#endif
