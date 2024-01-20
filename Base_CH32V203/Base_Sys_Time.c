#include "Base_Sys_Time.h"

/*
        SYS_Time
    功能在模块中实现，这里全是底层SDK
                                ———— 2022.7.7
*/

//* 底层 *//

#ifdef Exist_SYS_TIME
static uint64_t s_Tick_cnt;
static uint32_t s_Frequency;        //1s s_Tick_cnt 跑的数量,也就是 tick的频率
static uint32_t s_Frequency_us;     //1us s_Tick_cnt 跑的数量
static uint32_t s_Frequency_ms;     //1ms s_Tick_cnt 跑的数量

static uint32_t SysTick_Config(u64 ticks)
{
    // STK_CTLR = SysTick->CTLR
    SysTick->CTLR = (u32)0x00; // 关闭系统计数器STK，计数器停止计数

    SysTick->SR = (u32)0;
    SysTick->CNT = (u64)0;
    SysTick->CMP = ticks;
    //    NVIC_SetPriority(SysTicK_IRQn, 15);       //设置SysTick中断优先级
    //    NVIC_EnableIRQ(SysTicK_IRQn);             //使能开启Systick中断
    SysTick->CTLR = (u32)(0x29); // [向上计数] [0x29:8分频  0x2D:不开中断] 0x2F:开中断
    //    SysTick->CTLR |= (u32)(0x01 << 31);       // 中断触发使能

    s_Frequency = TICK_FREQUENCY;
    s_Frequency_us = s_Frequency / 1000000;
    s_Frequency_ms = s_Frequency / 1000;
    return (0);
}

#endif

void SYS_Time_Init(int Set)
{
#ifdef Exist_SYS_TIME
    if (Set)
    {
        if (SysTick_Config(TICK_SET_CMP)) // 系统使用滴答定时器
        {while (1);}
    }
    else
    {
        SysTick->CTLR = (u32)0x00;
    }
#endif
}

// Tick Controls
void SYS_Time_Set(Caven_BaseTIME_Type *time)
{
#ifdef Exist_SYS_TIME
    int temp;

    s_Tick_cnt = time->SYS_Sec;
    s_Tick_cnt *= s_Frequency;

    temp = time->SYS_Us;
    s_Tick_cnt += (temp * s_Frequency_us);

    SYSTICK_NUM = s_Tick_cnt;
#endif
}

void SYS_Time_Get(Caven_BaseTIME_Type *time)
{
#ifdef Exist_SYS_TIME
    int temp;
    s_Tick_cnt = SYSTICK_NUM;

    temp = s_Tick_cnt / s_Frequency;
    time->SYS_Sec = temp;

    temp = s_Tick_cnt % s_Frequency;
    temp /= s_Frequency_us;
    time->SYS_Us = temp;
#endif
}

// dog
void SYS_IWDG_Configuration(void)
{
#ifdef Exist_SYS_TIME

#endif
}

void SYS_Feed_Watchdog(void)
{
#ifdef Exist_SYS_TIME

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

void SYS_Delay_us(int n)
{
#ifdef Exist_SYS_TIME
    n = MIN(5000,n);
    u32 set_time = n * s_Frequency_us;
    s_Tick_cnt = SYSTICK_NUM;
    while (1)
    {
        if (SYSTICK_NUM > s_Tick_cnt)
        {
            if ((SYSTICK_NUM - s_Tick_cnt) >= set_time)
                break;
        }
        else
        {
            SysTick->SR = 0;    // 溢出了
            if ((SYSTICK_NUM + ((~((u64)0x00)) - s_Tick_cnt)) >= set_time)
                break;
        }
    }
#endif
}

void SYS_Delay_ms(int n)
{
#ifdef Exist_SYS_TIME
    n = MIN(5000,n);
    u32 set_time = n * s_Frequency_ms;      /* 其实u32 顶这个64位的8分频也只能顶 10s左右   */
    s_Tick_cnt = SYSTICK_NUM;
    while (1)
    {
        if (SYSTICK_NUM > s_Tick_cnt)
        {
            if ((SYSTICK_NUM - s_Tick_cnt) >= set_time)
                break;
        }
        else
        {
            SysTick->SR = 0;    // 溢出了
            if ((SYSTICK_NUM + ((~((u64)0x00)) - s_Tick_cnt)) >= set_time)
                break;
        }
    }
#endif
}

void SYS_Delay_S(int n)
{
    for (int var = 0; var < n; ++var)
    {
        SYS_Delay_ms(1000);
//        printf("1S \r\n");
    }
}

// Interrupt Handler
#ifdef Exist_SYS_TIME
static SYS_Time_Type SYS_Time;

void SYS_TIME_HANDLER()
{
    if (SYS_TIME_INTERRUPT_GETFLAG() == 1)
    {
        SYS_Time.SYS_Time_H ++;
        SYS_TIME_INTERRUPT_CLEANFLAG();
    }
}

#endif
