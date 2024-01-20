#ifndef _SYS_TIME_H__
#define _SYS_TIME_H__
/*
 *
本文件属于MCU专属文件，每个MCU不同，这里的内容就会不同 ———— 需要系统提供时钟
这个文件是给Time的，也就是说你得找一个能满足以下条件时钟来作为动力  （选一个幸运儿做苦力）。
-滴答定时器（首选） ch32v203的滴滴是64位，且可以向上计数
-通用定时器

为了方便上层统一调用，本文件需要提供以下函数
void SYS_Time_Init (int Set);

void SYS_Set_Tick (Caven_BaseTIME_Type * time);
void SYS_Get_Tick (Caven_BaseTIME_Type * time);

// Delay
void SYS_Base_Delay (int time,int Speed);

void SYS_Delay_us (int n);
void SYS_Delay_ms (int n);

//dog 函数内容可以为空
void SYS_IWDG_Configuration (void);
void SYS_Feed_Watchdog (void);

*/

#if DEFAULT
#include "Items.h"              //默认功能
#else
#include "User_items.h"         //自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"

/*
Tick_Set_CMP 不可以大于 2的24次方 16,777,216；也就是16M最大
*/



/* 【宏函数群】   */

#ifdef Exist_SYS_TIME

    #define TICK_FREQUENCY (MCU_SYS_FREQ)   //滴答分频（1分频）
    #define TICK_SET_CMP (TICK_FREQUENCY / 8)   //设置滴答初始值(/8 就是 1/8 s)
    #define TICK_OVER_IT (0.125)                 //定时器溢出时间（24位滴答才有）

    #define SYSTICK_NUM (SysTick->CNT)
    #define SYS_TIME_INTERRUPT_GETFLAG()    SysTick->SR
    #define SYS_TIME_INTERRUPT_CLEANFLAG()  SysTick->SR = 0

    #define SYS_TIME_HANDLER()  SysTick_Handler()
#endif


//很长的时间戳（一天）
typedef struct
{
    u32 SYS_Time_H; //每Frequency进1
    u32 SYS_Time_L; // 24bit 的
}SYS_Time_Type;

void SYS_Time_Init(int Set);

//
void SYS_Time_Set(Caven_BaseTIME_Type * time);
void SYS_Time_Get(Caven_BaseTIME_Type * time);

// Delay
void Base_SYS_Delay(int time, int speed);

void SYS_Delay_us(int n);
void SYS_Delay_ms(int n);

//dog
void SYS_IWDG_Configuration(void);
void SYS_Feed_Watchdog(void);

#endif
