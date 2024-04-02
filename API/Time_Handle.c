#include "Time_Handle.h"

/*
 * 这个是秒级的转换，基于Caven_Watch
 *
 */
int API_Hourly_to_Seconds(Caven_Watch_Type time)
{
    int temp;
    temp = time.second + (time.minutes * 60) + (time.hour * 3600); //设置的超时时差（秒级）
    temp = MIN(temp,86400);        // 60 * 60 * 24 = 86400
    return temp;
}

/*
 * 这个是 秒 级的转换
 *
 */
Caven_Watch_Type API_Seconds_to_Hourly(int Seconds)
{
    Caven_Watch_Type temp;
    int i;
    i = Seconds / 3600;
    temp.hour = MIN(i,24);
    
    i = (Seconds % 3600) / 60;
    temp.minutes = i;
    
    i = Seconds % 60;
    temp.second = i;
    
    return temp;
}

/*
 * temp_a < temp_b  :   retval = 1
 * temp_a = temp_b  :   retval = 0
 * temp_a > temp_b  :   retval = -1
 */
int API_Watch_Compare (Caven_Watch_Type temp_a,Caven_Watch_Type temp_b)
{
    int retval = 0;
    if (temp_b.hour < temp_a.hour)      // hour
    {
        retval = (-1);
    }
    else if (temp_b.hour > temp_a.hour)
    {
        retval = 1;
    }
    else if (temp_b.minutes < temp_a.minutes)   // minutes
    {
        retval = (-1);
    }
    else if (temp_b.minutes > temp_a.minutes)
    {
        retval = 1;
    }
    else if (temp_b.second < temp_a.second)     // second
    {
        retval = (-1);
    }
    else if (temp_b.second > temp_a.second)
    {
        retval = 1;
    }
    else if (temp_b.time_us < temp_a.time_us)   // time_us
    {
        retval = (-1);
    }
    else if (temp_b.time_us > temp_a.time_us)
    {
        retval = 1;
    }
    else if (temp_b.time_us == temp_a.time_us)
    {
        retval = 0;
    }
    else {
        retval = (-1);
    }
    return retval;
}

Caven_Watch_Type API_Watch_Get_differ (Caven_Watch_Type temp_a,Caven_Watch_Type temp_b)
{
    Caven_Watch_Type retval = {0};

    int i,j,k;
    i = API_Hourly_to_Seconds(temp_a);
    j = API_Hourly_to_Seconds(temp_b);
    if (i > j)
    {
        j += 86400;
    }
    k = j - i;

    i = temp_a.time_us;
    j = temp_b.time_us;
    if (i > j)
    {
        j += 1000000;
        k--;
    }
    retval = API_Seconds_to_Hourly (k);

    k = j - i;
    // MIN(k,1000000);
    retval.time_us = k;

    return retval;
}

/*
 * API:时间触发的任务（指Task标志位）
 */
int API_Task_Timer (Task_Overtime_Type *task,Caven_Watch_Type now_time)
{
    int retval = 0;
    int temp;

    Caven_Watch_Type diff_time;

    temp = API_Hourly_to_Seconds(task->Set_time);
    if (temp == 0 && task->Set_time.time_us == 0)   // time empty
    {
        retval = 2;
        return retval;
    }
    if (task->Switch == 1)
    {
        diff_time = API_Watch_Get_differ(task->Begin_time,now_time);
        temp = API_Watch_Compare (task->Set_time,diff_time);
        if (temp >= 0)
        {
            task->Flip_falg = !task->Flip_falg;
            task->Trigger_Flag = 1;
            task->Begin_time = now_time;
            retval = 1;
        }
        else
        {
            task->Trigger_Flag = 0;
        }
    }

    return retval;
}

