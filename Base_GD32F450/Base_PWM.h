#ifndef _BASE_PWM__H_
#define _BASE_PWM__H_

#ifdef DEFAULT
#include "Items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/****************/

/*
    AT32F415是没有TIM8的，但是对应的GPIO可以由TIM3的重映射来替代
                                                            ————2022.10.19
*/


void TIM3_PWM_Start_Init(int arr,int psc,int Set);
void TIM4_PWM_Start_Init(int arr,int psc,int Set);
void TIM8_PWM_Start_Init(int arr,int psc,int Set);

void TIM3_PWMx_SetValue (int channel,void *data);


#endif
