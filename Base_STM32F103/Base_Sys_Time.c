#include "Base_Sys_time.h"


#define LLTIMER_REG     (s_Frequency_CMP - SYSTICK_NUM)

#if Exist_SYS_TIME
static uint64_t s_Tick_cnt;
static uint32_t s_Frequency;        // 1s s_Tick_cnt 跑的数量,也就是 tick的频率
static uint32_t s_Frequency_CMP;    // 中断溢出需要的滴答值
static uint32_t s_Frequency_us;     // 1us s_Tick_cnt 跑的数量
static uint32_t s_Frequency_ms;     // 1ms s_Tick_cnt 跑的数量

static SYS_Time_Type s_SYS_Time = {0};

#endif

/*
    系统主频配置默认外部8m，倍频72m
*/
void SYS_Time_Init(int Set)
{
#if Exist_SYS_TIME
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    s_Frequency = TICK_FREQUENCY;
    s_Frequency_CMP = TICK_SET_CMP;
    s_Frequency_ms = (s_Frequency / 1000);
    s_Frequency_us = (s_Frequency / 1000000);
    if (Set)
    {
        // SysTick_Config(s_Frequency_CMP);
        if (s_Frequency_CMP > SysTick_LOAD_RELOAD_Msk)
        {
            while (1); 
        }
        else
        {
            // 禁用SysTick
            SysTick->CTRL = 0;
            // 设置重装载值
            SysTick->LOAD  = (s_Frequency_CMP & SysTick_LOAD_RELOAD_Msk) - 1;
            // 清除当前值
            SysTick->VAL = 0;
            // 配置SysTick使用8分频
            // 注意：HAL_SYSTICK_Config默认使用不分频时钟
            // 所以我们需要直接操作寄存器
            SysTick->CTRL = 0 << 2 |
                SysTick_CTRL_TICKINT_Msk |
                SysTick_CTRL_ENABLE_Msk;
        }
    }
    else
        SYS_RESET();

#endif
}

void SYS_Time_Set(SYS_BaseTIME_Type * time)
{
#if Exist_SYS_TIME
    uint64_t temp = 0;

    s_Tick_cnt = time->SYS_Sec;
    s_Tick_cnt *= s_Frequency;

    temp += (time->SYS_Us % 1000000);
    s_Tick_cnt += (uint64_t)(temp * s_Frequency_us);
	
	temp = (s_Tick_cnt % s_Frequency_CMP);
	SYSTICK_NUM = s_Frequency_CMP - temp;						// 载入寄存器
	
    s_SYS_Time.SYS_Time_H = s_Tick_cnt / s_Frequency_CMP;		// 高位设置
    s_SYS_Time.SYS_Time_L = temp & 0x00FFFFFF;                  // 低位设置
#endif
}

void SYS_Time_Get(SYS_BaseTIME_Type * time)
{
#if Exist_SYS_TIME
    uint32_t temp,cnt,cnt_us;
            
    cnt = s_SYS_Time.SYS_Time_H;
    s_SYS_Time.SYS_Time_L = LLTIMER_REG & 0x00FFFFFF;
    cnt_us = s_SYS_Time.SYS_Time_L;
    if(cnt < s_SYS_Time.SYS_Time_H)
    {
        cnt = s_SYS_Time.SYS_Time_H;
        s_SYS_Time.SYS_Time_L = LLTIMER_REG & 0x00FFFFFF;
        cnt_us = s_SYS_Time.SYS_Time_L;
    }
    temp = cnt % TICK_OVER_IT;
    if(temp)
    {
        temp = TICK_SET_CMP * temp;
    }
    temp += cnt_us;
    time->SYS_Sec = cnt >> 1;       // x/2,优化变成 x >> 1;
    //    time->SYS_Sec = cnt / TICK_OVER_IT;
    time->SYS_Us = temp / s_Frequency_us;
#endif
}

#if Exist_SYS_TIME
void SYS_TIME_HANDLER()
{
    s_SYS_Time.SYS_Time_H ++;
}
#endif


void SYS_IWDG_Configuration (void)
{
#if Exist_SYS_TIME

#endif
}

void SYS_Feed_Watchdog (void)
{
#if Exist_SYS_TIME

#endif
} 



void SYS_Delay_us(int n)
{
#if Exist_SYS_TIME
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
#if Exist_SYS_TIME
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
#if NOP
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
