#include "Time_Handle.h"

/*
 * API:时间触发的任务（指Task标志位）
 */
int API_Task_Timer (Task_Overtime_Type *task,Caven_BaseTIME_Type now_time)
{
    int retval = 0;
    int temp_num;
    Caven_BaseTIME_Type diff;
    if (task == NULL)
    {
        return (-1);
    }

    temp_num = task->Set_time.SYS_Sec;
    if (temp_num == 0 && task->Set_time.SYS_Us == 0)   // time empty
    {
        retval = 2;
        return retval;
    }
    if (task->Switch == 1)
    {
        temp_num = Caven_BaseTIME_Diff (now_time,task->Begin_time,&diff);
        task->error_code = 0;
        if (temp_num < 0)
        {
            retval = 3;
            task->Begin_time = now_time;
            task->error_code = 1;
            return retval;
        }
        else if(temp_num == 0)
        {
            return retval = 0;
        }
        else
        {
            if(diff.SYS_Sec > task->Set_time.SYS_Sec)
            {
                retval = 1;
                if((diff.SYS_Sec >> 1) > task->Set_time.SYS_Sec)
                {
                    task->Begin_time = now_time;
                    task->error_code = 2;
                    task->Flip_flag = !task->Flip_flag;
                    task->Trigger_flag = 1;
                    return retval;
                }
            }
            else if (diff.SYS_Sec == task->Set_time.SYS_Sec && diff.SYS_Us > task->Set_time.SYS_Us) {
                retval = 1;
            }
            if (retval > 0)
            {
                task->Begin_time.SYS_Sec += task->Set_time.SYS_Sec;
                task->Begin_time.SYS_Us += task->Set_time.SYS_Us;
                if(task->Begin_time.SYS_Us >= 1000000)
                {
                    task->Begin_time.SYS_Sec += 1;
                    task->Begin_time.SYS_Us = task->Begin_time.SYS_Us % 1000000;
                }
                task->Flip_flag = !task->Flip_flag;
                task->Trigger_flag = 1;
            }
            else
            {
                task->Trigger_flag = 0;
            }
        }
    }
    return retval;
}

// timezone_s 为时区参数(h)，例如北京时间 timezone_s = 8
struct tm API_UTC_Get_Date (int Unix,int timezone_s)
{
    struct tm retval = {0};
    struct tm *time_info;

    timezone_s *= 3600;
    time_t utc = Unix + timezone_s;
    
    time_info = localtime(&utc);
    if (time_info == NULL)
    {
        return retval;
    }
    memcpy(&retval,time_info,sizeof(struct tm));
    retval.tm_year += 1900;
    retval.tm_mon += 1;

    return retval;
}

/*
   retval < 0, more than expected;
   retval = 0,a = b;
   retval > 0,a > b;
*/
int Caven_BaseTIME_Diff (Caven_BaseTIME_Type a,Caven_BaseTIME_Type b,Caven_BaseTIME_Type *diff)
{
    int retval = 0;
    uint32_t sec_diff = 0;
    int us_diff = 0;
    if(diff == NULL)
    {
        return (-1);
    }
    else
    {
        diff->SYS_Sec = 0;
        diff->SYS_Us = 0;
    }
    if(a.SYS_Sec < b.SYS_Sec)
    {
        return (-1);
    }
    if (a.SYS_Sec == b.SYS_Sec && a.SYS_Us < b.SYS_Us)
    {
        return (-1);
    }

    sec_diff = a.SYS_Sec;
    sec_diff = sec_diff - b.SYS_Sec;
    us_diff = a.SYS_Us - b.SYS_Us;

    if (us_diff < 0)
    {
        sec_diff -= 1;
        us_diff += 1000000;
    }
    if(sec_diff > 0 || (sec_diff == 0 && us_diff > 0))
    {
        retval = 1;
        diff->SYS_Sec = sec_diff;
        diff->SYS_Us = us_diff % 1000000;
    }
    else
    {
        retval = 0;
        diff->SYS_Sec = 0;
        diff->SYS_Us = 0;
    }
    return retval;
}

/*
   retval < 0, more than expected;
   retval = 0,a = b;;
   retval > 0,a - b us diff max 33 minutes;
*/
int Caven_BaseTIME_Usdiff (Caven_BaseTIME_Type a,Caven_BaseTIME_Type b)
{
    int retval = 0;
    uint32_t us_diff = 0;
    Caven_BaseTIME_Type diff;
    retval = Caven_BaseTIME_Diff (a,b,&diff);
    if(retval > 0)
    {
        us_diff = diff.SYS_Sec;
        if(us_diff > 2000)
        {
            us_diff = 2000;
        }
        us_diff *= 1000000;
        us_diff += diff.SYS_Us;
        retval = (int)(us_diff);
    }
    else if(retval < 0)
    {
        retval = -1;
    }
    else
    {
        retval = 0;
    }

    return retval;
}
