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
    volatile Caven_BaseTIME_Type Set_time;
    volatile Caven_BaseTIME_Type Begin_time;
    volatile char Trigger_Flag;              //[000][1][000]
    volatile char Flip_falg;                 //[000][111][000]   only Read
    int Switch;
}Task_Overtime_Type;

int API_Task_Timer (Task_Overtime_Type *task,Caven_BaseTIME_Type now_time);

#endif
