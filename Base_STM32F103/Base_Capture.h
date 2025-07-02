#ifndef _BASE_CAPTURE__H_
#define _BASE_CAPTURE__H_

#ifdef DEFAULT
#include "Items.h"              //默认功能
#endif

#ifndef DEFAULT
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"

/****************/

#define TIM1_HANDLERIT() TIM1_CC_IRQHandler()
#define TIM2_HANDLERIT() TIM2_IRQHandler()
#define TIM3_HANDLERIT() TIM3_IRQHandler()
#define TIM4_HANDLERIT() TIM4_IRQHandler()
#define TIM5_HANDLERIT() TIM5_IRQHandler()

typedef struct
{
    char Channel;
	char finish_flag;
	int period_val;
	int high_val;
}TIM_Capture_Type;

void TIM1_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set);
void TIM2_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set);
void TIM3_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set);
void TIM5_Capture_Start_Init(int arr,int psc,char Channel,char mode,int Set);

int TIMx_Encoder_Capture(char TIMx);
void TIMx_Capture_Callback_pFunBind(char PWMx,D_pFun pFun);

#endif
