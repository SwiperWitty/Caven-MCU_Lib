#include "Time_Handle.h"

/*
 * API:时间触发的任务（指Task标志位）
 */
int API_Task_Timer (Task_Overtime_Type *task,Caven_BaseTIME_Type now_time)
{
    int retval = 0;
    int temp_num,temp_val;

    Caven_BaseTIME_Type diff_time;

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
        temp_num = now_time.SYS_Sec - task->Begin_time.SYS_Sec;
        task->error_code = 0;
        if (temp_num < 0)
        {
            retval = 3;
            task->Begin_time = now_time;
            task->error_code = 1;
            return retval;
        }
        else
        {
            temp_val = now_time.SYS_Us;
            if (temp_val < task->Begin_time.SYS_Us)
            {
                temp_num --;
                temp_val = temp_val + 1000000;
            }
            diff_time.SYS_Us = temp_val - task->Begin_time.SYS_Us;
            diff_time.SYS_Sec = temp_num;
        }
        temp_num = 0;
        if (diff_time.SYS_Sec > task->Set_time.SYS_Sec)
        {
            temp_num = 1;
        }
        else if (diff_time.SYS_Sec == task->Set_time.SYS_Sec)
        {
            if (diff_time.SYS_Us > task->Set_time.SYS_Us)
            {
                temp_num = 2;
            }
        }
        if (temp_num > 0)
        {
            task->Flip_flag = !task->Flip_flag;
            task->Trigger_flag = 1;
            if (task->Set_time.SYS_Sec > 0 &&
                diff_time.SYS_Sec >= (uint32_t)task->Set_time.SYS_Sec * 2U)
            {
                task->Begin_time = now_time;
                task->error_code = 2;
            }
            else
            {
                task->Begin_time.SYS_Sec += task->Set_time.SYS_Sec;
                task->Begin_time.SYS_Us += task->Set_time.SYS_Us;
            }
			if(task->Begin_time.SYS_Us >= 1000000)
			{
				task->Begin_time.SYS_Sec += 1;
				task->Begin_time.SYS_Us = task->Begin_time.SYS_Us % 1000000;
			}
            retval = 1;
        }
        else
        {
            task->Trigger_flag = 0;
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
   retval = 0,a = b;;
   retval > 0,a - b us diff;
*/
int Caven_BaseTIME_Diff (Caven_BaseTIME_Type a,Caven_BaseTIME_Type b)
{
    int retval = 0;
    int64_t sec_diff = 0;
    int64_t us_diff = 0;

    if(a.SYS_Sec < b.SYS_Sec)
    {
        retval = -1;
        return retval;
    }
    if (a.SYS_Sec == b.SYS_Sec && a.SYS_Us < b.SYS_Us)
    {
        return (-1);
    }

    sec_diff = (int64_t)a.SYS_Sec - (int64_t)b.SYS_Sec;
    if (sec_diff > 1000)
    {
        sec_diff = 1000;
    }

    us_diff = (int64_t)a.SYS_Us - (int64_t)b.SYS_Us;
    if (us_diff < 0)
    {
        sec_diff -= 1;
        us_diff += 1000000;
    }

    if (sec_diff < 0)
    {
        retval = -1;
    }
    else
    {
        retval = (int)(sec_diff * 1000000 + us_diff);
    }
    return retval;
}
