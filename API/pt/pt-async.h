#ifndef __PT_ASYNC_H__
#define __PT_ASYNC_H__
#include "pt.h"

struct pt_async {
    struct pt pt;
    struct pt_async* next;
    char (*fun)(struct pt *);
};
void async_add_pt(struct pt_async *async, struct pt_async *pt);
void async_del_pt(struct pt_async *async, struct pt_async *pt);
void async_poll(struct pt_async *async);


#ifdef PT_ASYNC_IMPLEMENTATION

void async_add_pt(struct pt_async *async, struct pt_async *pt)
{
    while (async->next != 0)
        async = async->next;
    async->next = pt;
}
void async_del_pt(struct pt_async *async, struct pt_async *pt)
{
    while ((async->next != 0)&&(async->next != pt))
        async = async->next;
    if(async->next != 0) 
        async->next = async->next->next;
}

void async_poll(struct pt_async *async)
{
    struct pt_async *a = async;
    do
    {
        if(async->fun)
            switch (async->fun(&async->pt))
            {
            case PT_WAITING:
            case PT_YIELDED:
            break;
            case PT_EXITED:
            case PT_ENDED:
                async_del_pt(a, async);
                break;
            default:
                break;
            }
        async = async->next;
    }while (async != 0);
}
#endif
#endif /* __PT_ASYNC_H__ */