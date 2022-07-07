#ifndef _TIME_H__
#define _TIME_H__

/*
    这个文件不依赖MCU的MDK（指编译能过）
    但是需要使用Base文件才能正常运行
*/

#include "base.h"
#include "Caven.h"

extern struct _SYS_Time SYS_Time;

struct _SYS_Time
{
    struct Caven_Date Date;
    struct Caven_Watch Watch;
    int sys_time;
};

struct Delay_
{
    void (*Delay_10us)(int num);
    void (*Delay_ms)(int num);
    void (*Delay_S)(char num);
};

void Time_Init(int SET);

void Delay_10us(int num);
void Delay_ms(int num);
void Delay_S(char num);


#endif
