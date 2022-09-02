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

u64 SysTick_merge (void)
{
    u64 temp;
    temp = SYS_Ticktime.SYS_Tick_H;
    SYS_Ticktime.SYS_Tick_L = 0;
    temp = (temp * Frequency) + SYS_Ticktime.SYS_Tick_L;
    return (temp);
}

void SYS_Delay_us (int n)
{
    u64 start_ticks,end_ticks;
    int set_time = n * (SystemCoreClock / 1000000);
    start_ticks = GET_SysTick;

    while(1)
    {
        end_ticks = GET_SysTick;
        if (end_ticks > start_ticks)
        {
            if ((end_ticks - start_ticks) >= set_time)
                break;
        }
        else
        {
            SysTick->SR = 0;    //溢出了
            if ((end_ticks + ((~((u64)0x00)) - start_ticks)) >= set_time)
                break;
        }
    }

}

void SYS_Delay_ms (int n)
{
    u64 start_ticks,end_ticks;
    int set_time = n * (SystemCoreClock / 1000);
    start_ticks = GET_SysTick;

    while(1)
    {
        end_ticks = GET_SysTick;
        if (end_ticks > start_ticks)
        {
            if ((end_ticks - start_ticks) >= set_time)
                break;
        }
        else
        {
            SysTick->SR = 0;    //溢出了
            if ((end_ticks + ((~((u64)0x00)) - start_ticks)) >= set_time)
                break;
        }
    }
}

void SYS_Delay_S (int n)
{
    for (int var = 0; var < n; ++var)
    {
        SYS_Delay_ms (1000);
        printf("1S \r\n");
    }
}
