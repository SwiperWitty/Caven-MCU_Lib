
#ifndef _TIME_H__
#define _TIME_H__

/*
    MODE_Time.h
    这个文件不依赖MCU的MDK（指编译能过）
    但是需要使用Base文件才能正常运行
    一般依赖于 SysTick & SysTick_Handler，缺一不可
    如果没有，即可使用普通定时器（16位及以上）
    

*/

#include "Base.h"
#include "API.h"

#define Exist_SYS_TIME

/*
 * (Date.Days + Watch) -> TIME -> Tick
 * 1.BaseTIME是所有TIME的基础，他的设置逻辑是 Days + Watch所有秒数+Watch.time_us
 * 2.BaseTIME是与BASE文件沟通的数据交互载体
 * 3.SYNC_Flag是写保护
 */
typedef struct
{
    Caven_Date_Type Date;
    Caven_Watch_Type Watch;
    volatile int SYNC_Flag;
}Real_TIME_Type;

//extern Real_TIME_Type Real_TIME;

typedef struct
{
    void (*Set_Date_pFun)(Caven_Date_Type time);
    void (*Set_Watch_pFun)(Caven_Watch_Type time);

    Caven_Date_Type (*Get_Date_pFun)(void);
    Caven_Watch_Type (*Get_Watch_pFun)(void);

    void (*Delay_Us)(int num);
    void (*Delay_Ms)(int num);
    void (*Delay_S) (int num);
}MODE_TIME_Way;


int MODE_TIME_Init(int SET);

void MODE_TIME_Set_Date_Fun(Caven_Date_Type time);
void MODE_TIME_Set_Watch_Fun(Caven_Watch_Type time);

Caven_Date_Type MODE_TIME_Get_Date_Fun(void);
Caven_Watch_Type MODE_TIME_Get_Watch_Fun(void);

void MODE_Delay_Us(int num);
void MODE_Delay_Ms(int num);
void MODE_Delay_S (int num);


#endif

