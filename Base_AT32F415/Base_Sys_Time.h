#ifndef _SYS_TIME_H__
#define _SYS_TIME_H__

#include "Items.h"

#define	Tick_Frequency	MCU_SYS_Freq		//滴答分频（不分频）
#define	Tick_Set_CMP	(Tick_Frequency/10) //设置滴答初始值(/10 就是 1/10 s)
#define	Tick_Over_IT	(0.10)  	        //定时器溢出时间（24位滴答才有）

#define Freq_us     (Tick_Frequency/1000000)   //微秒
#define Freq_ms     (Tick_Frequency/1000)      //毫秒
/* 【宏函数群】   */

//很长的时间戳（一天）
struct _SYS_Ticktime
{
    volatile uint32_t SYS_Tick_H;         //每Frequency进1 
    volatile uint32_t SYS_Tick_L;         //24bit 的
};

//source
uint64_t GET_SysTick (void);        //请在空闲时尽可能调用它
void SET_SysTick (uint64_t time);

//Init
void Sys_Time_Init (int Set);

//Delay
void SYS_Delay_us (int n);
void SYS_Delay_ms (int n);
void SYS_Delay_S (int n);


#endif

