#ifndef _BASE_PWM__H_
#define _BASE_PWM__H_

#ifdef DEFAULT
#include "items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

/****************/

/*

*/

#ifdef Exist_PWM
    #define TIM3_PWM
    #define TIM4_PWM
    #define TIM8_PWM
#endif

// arr是run_cnt溢出值，psc是对时钟源的分频,都可以是0 
void TIM3_PWM_Start_Init(int arr,int psc,int Set);
void TIM4_PWM_Start_Init(int arr,int psc,int Set);
void TIM8_PWM_Start_Init(int arr,int psc,int Set);

void TIM3_PWMx_SetValue(char PWMx,int value);
void TIM4_PWMx_SetValue(char PWMx,int value);
void TIM8_PWMx_SetValue(char PWMx,int value);


#endif
