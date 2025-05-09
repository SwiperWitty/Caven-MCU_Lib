#ifndef _MODE_TIME_H__
#define _MODE_TIME_H__

/*
    MODE_Time.h
    这个文件不依赖MCU的MDK（指编译能过）
    但是需要使用Base文件才能正常运行
    一般依赖于 SysTick,如果没有，即可使用普通定时器（16位及以上）
    
    base本质上提供的是sys_sec以及sys_us(SYS_BaseTIME_Type),mode层可以做date转utc，以及utc转date
    对base操作只允许给utc为参数。
    
*/

#include "Base.h"
#include "time.h"
#include "API.h"

#ifndef BASE_SYS_TIME_H__
typedef struct
{
    volatile uint32_t SYS_Sec;
    volatile uint32_t SYS_Us;       // 这里最大 1000 000
}SYS_BaseTIME_Type;

#endif
/*
 * (Date.Days + Watch) -> TIME -> Tick
 * 1.BaseTIME是所有TIME的基础，他的设置逻辑是 Days + Watch所有秒数+Watch.time_us
 * 2.BaseTIME是与BASE文件沟通的数据交互载体
 * 3.SYNC_Flag是写保护
 */
typedef struct
{
	volatile char SYNC_Flag;
	Caven_BaseTIME_Type Time;
    struct tm *date;
}Real_TIME_Type;

typedef struct
{
    void (*Set_BaseTIME_pFun)(Caven_BaseTIME_Type time);
    Caven_BaseTIME_Type (*Get_BaseTIME_pFun)(void);
    struct tm (*Get_Date_pFun) (int timezone_s);
    
    void (*Delay_Us)(int num);
    void (*Delay_Ms)(int num);
    void (*Delay_S) (int num);
}MODE_TIME_Way;


int MODE_TIME_Init(int Set);

void MODE_TIME_Set_BaseTIME (Caven_BaseTIME_Type time);
Caven_BaseTIME_Type MODE_TIME_Get_BaseTIME (void);
struct tm MODE_TIME_Get_Date (int timezone_s);

void MODE_Delay_Us(int num);
void MODE_Delay_Ms(int num);
void MODE_Delay_S (int num);


#endif

