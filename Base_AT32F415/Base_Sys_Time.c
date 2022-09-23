#include "base_sys_time.h"

#define Sys_Time_VAL    SysTick->VAL

void Sys_Time_Init(int Set)
{
#ifdef Exist_SYS_TIME       //这种保护不占内存，所以尽可能写
    if(Set)
    {
        if(SysTick_Config(Tick_Set_CMP))         //系统使用滴答定时器，因为RTC定时器的最小细分不足以用于一些场景
            while(1);
    }
    else
        NVIC_SystemReset();

#endif
}

struct _SYS_Ticktime SYS_Ticktime = {0};
void SysTick_Handler(void)
{
    SYS_Ticktime.SYS_Tick_H++;
}

//Tick_Set_CMP 是起点值（设置的），因为是24位自减寄存器
//这个返回的是，总系统滴答数，这个数是U64的，巨大
uint64_t GET_SysTick (void)
{
    uint64_t temp = 0;
    
    SYS_Ticktime.SYS_Tick_L = (Tick_Set_CMP - Sys_Time_VAL);   //滴答当前值
    temp = SYS_Ticktime.SYS_Tick_H;
    temp *= Tick_Set_CMP;                                      //乘法一定放后面，尤其是中断的东西
    temp += SYS_Ticktime.SYS_Tick_L;
    return (temp);
}

void SET_SysTick (uint64_t time)
{
    SYS_Ticktime.SYS_Tick_H = time / Tick_Set_CMP;             //高位设置
    SYS_Ticktime.SYS_Tick_L = (time % Tick_Set_CMP);           //低位设置(不设也行)
    Sys_Time_VAL = Tick_Set_CMP - SYS_Ticktime.SYS_Tick_L;     //载入低位
}

//Delay 
//
void SYS_Delay_us (int n)
{
    uint64_t start_ticks,end_ticks;     //都是滴答数，而非具体标准时间
    uint64_t temp;
    int set_time = n * Freq_us;
    start_ticks = GET_SysTick();

    while(1)
    {
        end_ticks = GET_SysTick();
        
        if (end_ticks > start_ticks)
        {
            temp = end_ticks - start_ticks;
        }
        else
        {
            temp = 86400 * Tick_Frequency;		//一天时间 * 滴答频率
            temp -= start_ticks;
            temp += end_ticks;
        }
        if (temp >= set_time)
        {
            break;
        }
    }

}

void SYS_Delay_ms (int n)
{
    uint64_t start_ticks,end_ticks;     //都是滴答数，而非具体标准时间
    uint64_t temp;
    int set_time = n * Freq_ms;
    start_ticks = GET_SysTick();
    
    while(1)
    {
        end_ticks = GET_SysTick();
        
        if (end_ticks > start_ticks)
        {
            temp = end_ticks - start_ticks;
        }
        else
        {
            temp = 86400 * Tick_Frequency;		//一天时间 * 滴答频率
            temp -= start_ticks;
            temp += end_ticks;
        }
        if (temp >= set_time)
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


