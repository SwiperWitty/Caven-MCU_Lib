#include "Base_Sys_time.h"

#define Sys_Time_VAL SysTick->VAL
int Tick_Full;       //提取宏，很多宏都是以运输的形式存在的，每次调用都会算一遍
int Freq_ms;
int Freq_us;

#ifdef Exist_SYS_TIME
static uint64_t s_Tick_cnt;
static uint32_t s_Frequency;        //1s s_Tick_cnt 跑的数量,也就是 tick的频率
static uint32_t s_Frequency_us;     //1us s_Tick_cnt 跑的数量
static uint32_t s_Frequency_ms;     //1ms s_Tick_cnt 跑的数量

#endif

void SYS_Time_Init(int Set)
{
#ifdef Exist_SYS_TIME
    Tick_Full = TICK_SET_CMP;
    Freq_ms = (TICK_FREQUENCY / 1000);
    Freq_us = (TICK_FREQUENCY / 1000000);
    if (Set)
    {
        if (SysTick_Config(TICK_SET_CMP)) // 系统使用滴答定时器
        {while (1);}
    }
    else
    {
        SYS_RESET();
    }
#endif
}

void SYS_Time_Set(Caven_BaseTIME_Type * time)
{
    
}

void SYS_Time_Get(Caven_BaseTIME_Type * time)
{
}

SYS_Time_Type SYS_Ticktime = {0};

#ifdef Exist_SYS_TIME
void SysTick_Handler(void)
{
    SYS_Ticktime.SYS_Time_H++;
}
#endif

// Tick_Set_CMP 是起点值（设置的），因为是24位自减寄存器
//这个返回的是，总系统滴答数，这个数是U64的，巨大


uint64_t GET_SysTick (SYS_Time_Type *stamp)
{
    uint64_t temp = 0;
#ifdef Exist_SYS_TIME
    SYS_Ticktime.SYS_Time_L = (Tick_Full - Sys_Time_VAL); //滴答当前值
    temp = SYS_Ticktime.SYS_Time_H;
    temp *= Tick_Full;                                             //乘法一定放后面，尤其是中断的东西
    temp += SYS_Ticktime.SYS_Time_L;
#endif
    return (temp);
}

void SET_SysTick(uint64_t time)
{
#ifdef Exist_SYS_TIME
    SYS_Ticktime.SYS_Time_H = time / Tick_Full;         //高位设置
    SYS_Ticktime.SYS_Time_L = (time % Tick_Full);       //低位设置(不设也行)
    Sys_Time_VAL = Tick_Full - SYS_Ticktime.SYS_Time_L; //载入低位
#endif
}

// Delay
//
void SYS_Delay_us(int n)
{
    uint64_t start_ticks, end_ticks; //都是滴答数，而非具体标准时间
    uint64_t temp;
    int set_time = n * Freq_us;
    SYS_Time_Type stamp;
    start_ticks = GET_SysTick(&stamp);
#ifdef Exist_SYS_TIME
    while (1)
    {
        end_ticks = GET_SysTick(&stamp);

        if (end_ticks > start_ticks)
        {
            temp = end_ticks - start_ticks;
        }
        else
        {
            temp = 86400 * TICK_FREQUENCY; //一天时间 * 滴答频率
            temp -= start_ticks;
            temp += end_ticks;
        }
        if (temp >= set_time)
        {
            break;
        }
    }
#endif
}

void SYS_Delay_ms(int n)
{
    uint64_t start_ticks, end_ticks; //都是滴答数，而非具体标准时间
    uint64_t temp;
    int set_time = n * Freq_ms;
    SYS_Time_Type stamp;
    start_ticks = GET_SysTick(&stamp);
#ifdef Exist_SYS_TIME
    while (1)
    {
        end_ticks = GET_SysTick(&stamp);

        if (end_ticks > start_ticks)
        {
            temp = end_ticks - start_ticks;
        }
        else
        {
            temp = 86400 * TICK_FREQUENCY; //一天时间 * 滴答频率
            temp -= start_ticks;
            temp += end_ticks;
        }
        if (temp >= set_time)
        {
            break;
        }
    }
#endif
}

// delay
void Base_SYS_Delay(int time, int speed)
{
#ifdef NOP
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = speed; // SET
        do
        {
            NOP();
        } while ((temp--) > 0);
    }
#endif
}
