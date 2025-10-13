/*
 * Caven_event_frame.h
 *
 *  Created on: 2024
 */

#ifndef CAVEN_EVENT_FRAME_H_
#define CAVEN_EVENT_FRAME_H_

#include "Caven_Type.h"

#define CAVEN_EVENTS_MAX    20

/*
    此结构体包含指针变量，所以此结构体需要初始化其中的指针。
	其中 D_pFun event_pFun 函数，传参为void *data，默认为int；调用完函数之后，如果传参变成了0，那么这个事件就结束了。
*/
typedef struct
{
    int events_num;                         // 事件总数量
    unsigned char events[CAVEN_EVENTS_MAX]; // 事件处理标志位
    D_pFun events_pFun[CAVEN_EVENTS_MAX];   // 事件执行函数

}Caven_event_Type;


int Caven_new_event_Fun(Caven_event_Type *events,D_pFun event_pFun,int *handle);
int Caven_delete_event_Fun(Caven_event_Type *events,int *handle);

int Caven_trigger_event_Fun(Caven_event_Type *events,int const handle,char data);

int Caven_handle_event_Fun(Caven_event_Type *events);

#endif /* CAVEN_EVENT_FRAME_H_ */
