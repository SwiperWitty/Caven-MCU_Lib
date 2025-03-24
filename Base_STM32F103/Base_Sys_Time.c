#include "Base_Sys_time.h"


#define LLTIMER_REG     (s_Frequency_CMP - SYSTICK_NUM)

#ifdef Exist_SYS_TIME
static uint64_t s_Tick_cnt;
static uint32_t s_Frequency;        // 1s s_Tick_cnt 跑的数量,也就是 tick的频率
static uint32_t s_Frequency_CMP;    // 中断溢出需要的滴答值
static uint32_t s_Frequency_us;     // 1us s_Tick_cnt 跑的数量
static uint32_t s_Frequency_ms;     // 1ms s_Tick_cnt 跑的数量

static SYS_Time_Type s_SYS_Time = {0};

#endif

void SYS_Time_Init(int Set)
{
#ifdef Exist_SYS_TIME
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    s_Frequency = TICK_FREQUENCY;
    s_Frequency_CMP = TICK_SET_CMP;
    s_Frequency_ms = (s_Frequency / 1000);
    s_Frequency_us = (s_Frequency / 1000000);
    if (Set)
    {
        if (SysTick_Config(TICK_SET_CMP)) // 系统使用滴答定时器
        {while (1);}
    }
    else
        SYS_RESET();

#endif
}

void SYS_Time_Set(SYS_BaseTIME_Type * time)
{
#ifdef Exist_SYS_TIME
    int temp = 0;

    s_Tick_cnt = time->SYS_Sec;
    s_Tick_cnt *= s_Frequency;

    temp += time->SYS_Us;
    s_Tick_cnt += (uint64_t)(temp * s_Frequency_us);
	
	temp = (s_Tick_cnt % s_Frequency_CMP);
	SYSTICK_NUM = s_Frequency_CMP - temp;						// 载入寄存器
	
    s_SYS_Time.SYS_Time_H = s_Tick_cnt / s_Frequency_CMP;		// 高位设置
    s_SYS_Time.SYS_Time_L = temp;       						// 低位设置
#endif
}

void SYS_Time_Get(SYS_BaseTIME_Type * time)
{
#ifdef Exist_SYS_TIME
    int temp = 0;

    s_SYS_Time.SYS_Time_L = LLTIMER_REG;
    if (s_SYS_Time.SYS_Time_H % 2)
    {
        temp += s_Frequency_CMP;
    }
    temp += s_SYS_Time.SYS_Time_L;

    time->SYS_Us = temp / s_Frequency_us;
    time->SYS_Sec = s_SYS_Time.SYS_Time_H * TICK_OVER_IT;   // x/2,优化变成 x >> 1;
#endif
}

#ifdef Exist_SYS_TIME
void SYS_TIME_HANDLER()
{
    s_SYS_Time.SYS_Time_H ++;
}
#endif


void SYS_IWDG_Configuration (void)
{
#ifdef Exist_SYS_TIME

#endif
}

void SYS_Feed_Watchdog (void)
{
#ifdef Exist_SYS_TIME

#endif
} 



void SYS_Delay_us(int n)
{
#ifdef Exist_SYS_TIME
    n = MIN(5000,n);
    uint32_t set_Tick_cnt = n * s_Frequency_us;
    uint64_t start_Tick_cnt;
    s_Tick_cnt = s_SYS_Time.SYS_Time_H;
    s_Tick_cnt *= s_Frequency_CMP;
    s_Tick_cnt += LLTIMER_REG;
    start_Tick_cnt = s_Tick_cnt;
    while (1)
    {
        s_Tick_cnt = s_SYS_Time.SYS_Time_H;
        s_Tick_cnt *= s_Frequency_CMP;
        s_Tick_cnt += LLTIMER_REG;
        if ((s_Tick_cnt - start_Tick_cnt) > set_Tick_cnt)
        {
            break;
        }
        else
        {

        }
    }
#endif
}

void SYS_Delay_ms(int n)
{
#ifdef Exist_SYS_TIME
    n = MIN(5000,n);
    uint32_t set_Tick_cnt = n * s_Frequency_ms;      /* 其实u32 顶这个64位的8分频也只能顶 10s左右   */
    uint64_t start_Tick_cnt;
    s_Tick_cnt = s_SYS_Time.SYS_Time_H;
    s_Tick_cnt *= s_Frequency_CMP;
    s_Tick_cnt += LLTIMER_REG;
    start_Tick_cnt = s_Tick_cnt;
    while (1)
    {
        s_Tick_cnt = s_SYS_Time.SYS_Time_H;
        s_Tick_cnt *= s_Frequency_CMP;
        s_Tick_cnt += LLTIMER_REG;
        if ((s_Tick_cnt - start_Tick_cnt) > set_Tick_cnt)
        {
            break;
        }
        else
        {
            
        }
    }
#endif
}

// Delay
/*
    a * b 个 NOP
*/
void SYS_Base_Delay (int time,int Speed)
{
#ifdef NOP
    volatile int temp;
    for (int i = 0; i < time; ++i)
    {
        temp = Speed;            //SET
        do{
            NOP();
        }while((temp--) > 0);
    }
#endif
}
