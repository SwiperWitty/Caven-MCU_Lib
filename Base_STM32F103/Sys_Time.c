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
        if(SysTick_Config(Frequency))         //系统使用滴答定时器，因为RTC定时器的最小细分不足以用于一些场景
            while(1);
    }
    else
        NVIC_SystemReset();

    #endif
#endif
}

volatile struct _SYS_Ticktime SYS_Ticktime = {0};

void SysTick_Handler(void)
{
    SYS_Ticktime.SYS_Tick_H++;

}

uint64_t SysTick_Merge (void)
{
    uint64_t temp;
    temp = SYS_Ticktime.SYS_Tick_H;
    SYS_Ticktime.SYS_Tick_L = (Frequency - SysTick->VAL);     //滴答当前值
    temp = (temp * Frequency) + SYS_Ticktime.SYS_Tick_L;
    return (temp);
}

void SysTick_Reload (uint64_t time)
{
    SYS_Ticktime.SYS_Tick_H = time / Frequency;             //高位设置
    SYS_Ticktime.SYS_Tick_L = (time % Frequency);           //低位设置(不设也行)
    SysTick->VAL = Frequency - SYS_Ticktime.SYS_Tick_L;     //载入低位
}

void SYS_Delay_us (int n)
{
    uint64_t start_ticks,end_ticks;
    int set_time = n * (SystemCoreClock / 1000000);
    start_ticks = GET_SysTick();

    while(1)
    {
        end_ticks = GET_SysTick();
        if (end_ticks > start_ticks)
        {
            if ((end_ticks - start_ticks) >= set_time)
                break;
        }
        else
        {
            break;
        }
    }

}

void SYS_Delay_ms (int n)
{
    uint64_t start_ticks,end_ticks;
    int set_time = n * (SystemCoreClock / 1000);
    start_ticks = GET_SysTick();

    while(1)
    {
        end_ticks = GET_SysTick();
        if (end_ticks > start_ticks)
        {
            if ((end_ticks - start_ticks) >= set_time)
                break;
        }
        else
        {
            break;
        }
    }
}

void SYS_Delay_S (int n)
{
    for (int var = 0; var < n; ++var)
    {
        SYS_Delay_ms (1000);
        
    }
}
