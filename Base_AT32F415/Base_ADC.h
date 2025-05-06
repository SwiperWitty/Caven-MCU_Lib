#ifndef BASE_ADC__H_
#define BASE_ADC__H_

#ifdef DEFAULT
#include "items.h"              // 默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"

/****************/

#ifdef Exist_ADC
    extern float VDDA_num;
    
    #define ADC_MAX                          4096   
    #define ADC_VREF                         VDDA_num
    #define ADC_TEMP_BASE                    (1.26)
    #define ADC_TEMP_SLOPE                   (-0.00423)

    #define ADC_Speed ADC_SAMPLETIME_239_5

    #define ADC_IO_PA0  ADC_CHANNEL_0   // tim5
    #define ADC_IO_PA1  ADC_CHANNEL_1   // tim5 OPEN_0001
    #define ADC_IO_PA2  ADC_CHANNEL_2   // tx2
    #define ADC_IO_PA3  ADC_CHANNEL_3   // rx2  OPEN_0010
    #define ADC_IO_PA4  ADC_CHANNEL_4   // ADC1
    #define ADC_IO_PA5  ADC_CHANNEL_5   // ADC2 OPEN_0100
    #define ADC_IO_PA6  ADC_CHANNEL_6   // tim3
    #define ADC_IO_PA7  ADC_CHANNEL_7   // tim3 OPEN_1000

    #define ADC_Temp    ADC_CHANNEL_16  // 温传
    
    #define ADC_FINISH_HANDLERIT() DMA1_Channel1_IRQHandler()
#endif

// 面向对象编程

typedef struct
{
    float (*Get_MCU_Temperature_pFun)(void);
    float (*Conversion_Vol_pFun)(int Num);
    
    void (*Receive_Bind_pFun)(D_pFun ADC_pFun);
}MODE_USER_ADC_Way;

int Base_ADC_Init (int Set);

float ADC_Conversion_Vol_Fun (int Num);
float ADC_Get_MCU_Temperature_Fun (void);
void ADC_Quick_Get_Bind_Fun (D_pFun ADC_pFun);

#endif 
