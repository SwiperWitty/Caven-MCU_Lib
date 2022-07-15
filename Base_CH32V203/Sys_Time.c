#include "sys_time.h"

/*
        SYS_Time
    功能在模块中实现，这里全是底层SDK
                                ———— 2022.7.7
*/


//* 底层 *//
void Sys_Time_Init (int Set)
{
#ifdef Exist_SYS_TIME
    #ifdef Base_SysTick
    if(Set)
    {
        if(SysTick_Config(SystemCoreClock/Frequency))         //系统使用滴答定时器，因为RTC定时器的最小细分不足以用于一些场景
            while(1);
    }
    else
        NVIC_SystemReset();
    
    #else
    if(Set)
    {
        TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
        NVIC_InitTypeDef NVIC_InitStructure = {0};
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

        TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseInitStructure.TIM_Period = 1;
        TIM_TimeBaseInitStructure.TIM_Prescaler = 718;
        TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
        TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure); //时钟配置

        NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
        NVIC_Init(&NVIC_InitStructure);

        TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);          //使能中断
        TIM_Cmd(TIM4, ENABLE);
    }
    else
        NVIC_SystemReset();

    #endif
#endif
}


