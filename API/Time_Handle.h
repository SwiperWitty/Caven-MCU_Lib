#ifndef _TIME_HANDLE__H_
#define _TIME_HANDLE__H_

/*
    Time_Handle.h
        【API】      ————C语言环境即可运行
    Over_Time       数据超时函数
    Over_Time增加稳定性
                            ————2022年9月5日
    
*/

#include "Caven_Type.h"

typedef struct
{
    volatile Caven_Watch_Type Set_time;
    volatile Caven_Watch_Type Begin_time;
    volatile char Trigger_Flag;              //[000][1][000]
    volatile char Flip_falg;                 //[000][111][000]   only Read
    int Switch;
}Task_Overtime_Type;


int API_Hourly_to_Seconds(Caven_Watch_Type time);

Caven_Watch_Type API_Seconds_to_Hourly (int Seconds);
Caven_Watch_Type API_Watch_Get_differ (Caven_Watch_Type temp_a,Caven_Watch_Type temp_b);

int API_Watch_Compare (Caven_Watch_Type temp_a,Caven_Watch_Type temp_b);

int API_Task_Timer (Task_Overtime_Type *task,Caven_Watch_Type now_time);

#endif
