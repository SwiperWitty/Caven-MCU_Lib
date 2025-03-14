#ifndef _SYS_TIME_H__
#define _SYS_TIME_H__
#if __cplusplus
extern "C"
{
#endif
/*
 *
本文件属于MCU专属文件，每个MCU不同，这里的内容就会不同 ———— 需要系统提供时钟
这个文件是给Time的，也就是说你得找一个能满足以下条件时钟来作为动力  （选一个幸运儿做苦力）。
-滴答定时器（首选） ch32v203的滴滴是64位，且可以向上计数
-通用定时器

为了方便上层统一调用，本文件需要提供以下函数
void SYS_Time_Init (int Set);

void SYS_Time_Set (SYS_BaseTIME_Type * time);
void SYS_Time_Get (SYS_BaseTIME_Type * time);

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

/*
Tick_Set_CMP 不可以大于 2的24次方 16,777,216；也就是16M最大
*/



/* 【宏函数群】   */

#ifdef Exist_SYS_TIME

    #define TICK_FREQUENCY (MCU_SYS_FREQ / 8)   //滴答分频（8分频）
    #define TICK_SET_CMP (TICK_FREQUENCY / 2)   //设置滴答初始值(/2 就是 1/2 s)
    #define TICK_OVER_IT (0.50)                 //定时器溢出时间（24位滴答才有）

    #define SYSTICK_NUM (SysTick->VAL)

    #define SYS_TIME_HANDLER()  SysTick_Handler()
#endif


// 很长的时间戳（一天）
typedef struct
{
    uint32_t SYS_Time_H; // 每Frequency进1
    uint32_t SYS_Time_L; // 24bit 的
}SYS_Time_Type;
// 系统运行总时长 8byte
typedef struct
{
    uint32_t SYS_Sec;
    uint32_t SYS_Us;             // 这里最大 1000 000
}SYS_BaseTIME_Type;

// Init (systick(Set = 1/2)  dog(Set = 2))
void SYS_Time_Init(int Set);        //enable & reboot

//
void SYS_Time_Set(SYS_BaseTIME_Type * time);
void SYS_Time_Get(SYS_BaseTIME_Type * time);

// Delay
void SYS_Base_Delay(int time, int speed);

void SYS_Delay_us(int n);
void SYS_Delay_ms(int n);

//dog
void SYS_IWDG_Configuration(void);
void SYS_Feed_Watchdog(void);
#if __cplusplus
}
#endif

#endif
