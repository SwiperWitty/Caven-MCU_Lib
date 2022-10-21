#ifndef _BASE_PWM__H_
#define _BASE_PWM__H_

#include "Items.h"

/*
    AT32F415是没有TIM8的，但是对应的GPIO可以由TIM3的重映射来替代
                                                            ————2022.10.19
*/

#ifdef Exist_PWM
    #define TIM3_PWM
    #define TIM4_PWM
//    #define TIM8_PWM
#endif

void TIM3_PWM_Start_Init(int arr,int psc,int Set);
void TIM4_PWM_Start_Init(int arr,int psc,int Set);
//void TIM8_PWM_Start_Init(int arr,int psc,int Set);

void TIM3_PWMx_SetValue(char PWMx,int value);
void TIM4_PWMx_SetValue(char PWMx,int value);
//void TIM8_PWMx_SetValue(char PWMx,int value);


#endif
