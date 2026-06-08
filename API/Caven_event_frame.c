/*
 * Caven_event_frame.c
 *
 *  Created on: 2024
 */
#include "Caven_event_frame.h"

/*
 * Caven_new_event_Fun
 * 1.首先你得有个events
 * 2.符合结构、函数指针
 * 3.句柄带0xEExx说明存在even,但是事件检测器events的最高位0x80才是最高判断标准。
 */
int Caven_new_event_Fun(Caven_event_Type *events,D_pFun event_pFun,int *handle)
{
    int retval = 0;
    Caven_event_Type temp_events;
    memcpy(&temp_events,events,sizeof(Caven_event_Type));

    if (events == NULL || event_pFun == NULL || handle == NULL)
    {
        retval = (-1);
        return retval;
    }
    if (temp_events.events_num >= CAVEN_EVENTS_MAX)
    {
        retval = (-2);
        return retval;
    }
    if (*handle != 0)
    {
        retval = (-3);
    }
    else
    {
        temp_events.events_num ++;
        temp_events.events_num &= 0xfff;
        *handle = temp_events.events_num | 0xE000;              // 有句柄，说明事件存在
        temp_events.events[temp_events.events_num] = CAVEN_EVENTS_ALIVE;      // 最高位为1，说明事件存在
        temp_events.events_pFun[temp_events.events_num] = event_pFun;
        memcpy(events,&temp_events,sizeof(Caven_event_Type));
    }

    return retval;
}

int Caven_delete_event_Fun(Caven_event_Type *events,int *handle)
{
    int retval = 0;
    int temp_num;

    if (events == NULL || handle == NULL)
    {
        retval = (-1);
        return retval;
    }
    if (events->events_num >= CAVEN_EVENTS_MAX)
    {
        retval = (-2);
    }
    else if(events->events_num > 0)
    {
        temp_num = *handle;
        temp_num &= 0xfff;
        events->events_pFun[temp_num] = NULL;
        events->events[temp_num] = 0x00;
        *handle = 0;
        events->events_num --;
    }

    return retval;
}

/*
 * handle的低8位是任务号，高8位是E0说明这个事件被初始化了，句柄有效。
 * (temp_events.events[temp_num] & 0x80) > 0,说明事件有绑定执行函数。
 * (temp_events.events[temp_num] & 0x40) > 0,说明事件被触发，想结束这个事件，只能由触发的执行函数将其传入指针内容清0
 * data只有6位有效值,为0则无效
 */
int Caven_trigger_event_Fun(Caven_event_Type *events,int const handle,char data)
{
    int retval = 0;
    int temp_num;

    if (events == NULL)
    {
        retval = (-1);
        return retval;
    }
    if ((handle & 0xe000) && data != 0)   // 有句柄，说明事件存在
    {
        temp_num = handle;
        temp_num &= 0xff;
        if (events->events[temp_num] & CAVEN_EVENTS_ALIVE)    // 有绑定函数
        {
            events->events[temp_num] = ((data & 0x3f) | CAVEN_EVENTS_ALIVE | CAVEN_EVENTS_RUN);     // 触发事件并传递变量
        }
    }
    else
    {
        retval = (-2);
    }

    return retval;
}

/*
    status = CAVEN_EVENTS_STOP/0，stop；
    status = 1，run；
*/
int Caven_suspend_event_Fun(Caven_event_Type *events,int *handle,int status)
{
    int retval = 0;
    int temp_num;

    if (events == NULL || handle == NULL)
    {
        retval = (-1);
        return retval;
    }
    else if(events->events_num > 0)
    {
        temp_num = *handle;
        temp_num &= 0xfff;
        if(status == CAVEN_EVENTS_STOP)
        {
            events->events[temp_num] &= ~CAVEN_EVENTS_RUN;
        }
        else
        {
            events->events[temp_num] |= CAVEN_EVENTS_RUN;
        }
    }
    return retval;
}

/*
 * Caven_handle_event_Fun
 * 1.使用前需要先调用Caven_new_event_Fun
 * 2.Caven_handle_event_Fun函数应该放在状态机里面轮番调用
 * 3.events里面的events_pFun，必须无阻塞。
 * 4.通过Caven_trigger_event_Fun激活，data(前6位有效)会将参数给events保存，events_pFun里面需要将data置0，结束当前触发。
 */
int Caven_handle_event_Fun(Caven_event_Type *events)
{
    int retval = 0;
    int temp;
    unsigned char temp_num = events->events_num;

    if (events == NULL)
    {
        retval = (-1);
        return retval;
    }

    if (temp_num > CAVEN_EVENTS_MAX)
    {
        retval = (-2);
        return retval;
    }
    for (int i = 0; i <= temp_num; ++i)
    {
        temp = events->events[i] & 0xff;
        if (temp & CAVEN_EVENTS_ALIVE)          /* 存在句柄  */
        {
            if (temp & CAVEN_EVENTS_RUN)        /* 存在触发  */
            {
                temp &= 0x3F;
                if (events->events_pFun[i] != NULL)
                {
                    events->events_pFun[i](&temp);          /* 执行指针函数  */
                }
                if (temp == 0)      // Exit
                {
                    events->events[i] = CAVEN_EVENTS_ALIVE; /* 清除触发标记  */
                }
            }
        }
    }
    return retval;
}




