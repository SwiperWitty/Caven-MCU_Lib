#ifndef _ADC_H_
#define _ADC_H_

/*
 *  Base_ADC.h
 *
*/

#if DEFAULT
#include "Items.h"              //默认功能
#else
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

#define ADC_0 ADC_Channel_0
#define ADC_1 ADC_Channel_1
#define ADC_2 ADC_Channel_2
#define ADC_3 ADC_Channel_3

#define ADC_8 ADC_Channel_8
#define ADC_9 ADC_Channel_9

#define MCU_Temp ADC_Channel_TempSensor

#define ADC_De_Time ADC_SampleTime_28Cycles5


struct ADC_
{
	float (*Get_Vol)(char ADC_x);
	float (*Get_MCU_Temp)(void);
};

void ADCx_Init(char ADC_x, int Set);
float Get_ADCx_Vol(uint8_t ADC_x);
float Get_MCU_Temp(void);

#endif
