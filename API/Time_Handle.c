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
 * API:时间触发的任务（指Task标志位）
 */
int API_Task_Timer (Task_Overtime_Type *task,Caven_BaseTIME_Type now_time)
{
    int retval = 0;
    int temp_num,temp_val;

    Caven_BaseTIME_Type diff_time;

    temp_num = task->Set_time.SYS_Sec;
    if (temp_num == 0 && task->Set_time.SYS_Us == 0)   // time empty
    {
        retval = 2;
        return retval;
    }
    if (task->Switch == 1)
    {
        temp_num = now_time.SYS_Sec - task->Begin_time.SYS_Sec;
        if (temp_num < 0)
        {
            retval = 3;
            task->Begin_time = now_time;
            return retval;
        }
        else
        {
            temp_val = now_time.SYS_Us;
            if (now_time.SYS_Us < task->Begin_time.SYS_Us)
            {
                temp_num --;
                temp_val = now_time.SYS_Us + 1000000;
            }
            diff_time.SYS_Us = temp_val - task->Begin_time.SYS_Us;
            diff_time.SYS_Sec = temp_num;
        }
        temp_num = 0;
        if (diff_time.SYS_Sec > task->Set_time.SYS_Sec)
        {
            temp_num = 1;
        }
        else if (diff_time.SYS_Sec >= task->Set_time.SYS_Sec)
        {
            if (diff_time.SYS_Us >= task->Set_time.SYS_Us)
            {
                temp_num = 2;
            }
        }
        if (temp_num > 0)
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

