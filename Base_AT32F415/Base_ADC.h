#ifndef BASE_ADC__H_
#define BASE_ADC__H_

#include "Items.h"

/*
    使用DMA来读ADC的好处是一次可以获取所有的数据。
    而且快速
*/

#define ADC_Speed ADC_SAMPLETIME_239_5

#define ADC_IO_PA0  ADC_CHANNEL_0
#define ADC_IO_PA1  ADC_CHANNEL_1
#define ADC_IO_PA2  ADC_CHANNEL_2
// #define ADC_IO_PA3  ADC_CHANNEL_3
// #define ADC_IO_PA4  ADC_CHANNEL_4
// #define ADC_IO_PA5  ADC_CHANNEL_5
// #define ADC_IO_PA6  ADC_CHANNEL_6
// #define ADC_IO_PA7  ADC_CHANNEL_7
#define ADC_IO_PB0  ADC_CHANNEL_8
#define ADC_IO_PB1  ADC_CHANNEL_9


void ADC_Start_Init(int Set);
void ADC_Get_List(int *Target);

#endif 
