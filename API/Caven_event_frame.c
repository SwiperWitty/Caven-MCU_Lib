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

    if (events == NULL || event_pFun == NULL)
    {
        retval = (-1);
        return retval;
    }
    if (temp_events.events_num >= CAVEN_EVENTS_MAX)
    {
        retval = (-2);
    }
    else
    {
        temp_events.events_num ++;
        *handle = temp_events.events_num | 0xEE00;              // 有句柄，说明事件存在
        temp_events.events[temp_events.events_num] = 0x80;      // 最高位为1，说明事件存在
        temp_events.events_pFun[temp_events.events_num] = event_pFun;
        memcpy(events,&temp_events,sizeof(Caven_event_Type));
    }

    return retval;
}

int Caven_delete_event_Fun(Caven_event_Type *events,int *handle)
{
    int retval = 0;
    Caven_event_Type temp_events;
    int temp_num;
    memcpy(&temp_events,events,sizeof(Caven_event_Type));

    if (events == NULL)
    {
        retval = (-1);
        return retval;
    }
    if (temp_events.events_num >= CAVEN_EVENTS_MAX)
    {
        retval = (-2);
    }
    else
    {
        temp_num = *handle;
        temp_num &= 0xff;
        temp_events.events_pFun[temp_num] = NULL;
        temp_events.events[temp_num] = 0x00;
        *handle = 0;
        temp_events.events_num --;
        memcpy(events,&temp_events,sizeof(Caven_event_Type));
    }

    return retval;
}

int Caven_trigger_event_Fun(Caven_event_Type *events,int const handle,char data)
{
    int retval = 0;
    Caven_event_Type temp_events;
    int temp_num;
    memcpy(&temp_events,events,sizeof(Caven_event_Type));

    if (events == NULL)
    {
        retval = (-1);
        return retval;
    }
    if (handle & 0xee00)              // 有句柄，说明事件存在
    {
        temp_num = handle;
        temp_num &= 0xff;
        temp_events.events[temp_num] |= ((data & 0x3f) + 0x40);     // 触发事件并传递变量
        memcpy(events,&temp_events,sizeof(Caven_event_Type));
    }
    else
    {
        retval = (-2);
    }

    return retval;
}

/*
 * Caven_handle_event_Fun
 * 1.使用前需要先调用Caven_new_event_Fun
 * 2.Caven_handle_event_Fun函数应该放在状态机里面轮番调用
 * 3.events里面的events_pFun，必须无阻塞。
 * 4.通过Caven_trigger_event_Fun激活，data会将参数给events保存，events_pFun里面需要将data置0，结束当前触发。
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
        if (temp & 0x80)        /* 存在句柄  */
        {
            if (temp & 0x40)    /* 存在触发  */
            {
                temp &= 0x3F;
                if (events->events_pFun[i] != NULL)
                {
                    events->events_pFun[i](&temp);          /* 执行指针函数  */
                }
                if (temp == 0)
                {
                    events->events[i] &= 0x80;              /* 清除触发标记  */
                }
            }
        }
    }
    return retval;
}




