#ifndef _TCP_SERVER_LINK__H_
#define _TCP_SERVER_LINK__H_

#include "stdint.h"
#include "Network_manage.h"

/*
    1).默认已经连接上网络(掉网时，阻塞的函数会让网络自动重置)
    2).发送是操作文件的型式发送[tcp_server_send_data]，和MCU类似。
    3).接收有两种方式，一种是[tcp_server_Receive_Get_Fun]，他是非阻塞接收的，也就是说他需要进线程才能收到数据
    另一种是开一个多线程任务[tcp_server_link_task]去阻塞接收，当然你也可以在此线程中使用第一种方式，如果要阻塞接收需使用[tcp_server_receive_State_Machine_Bind]
    绑定一个数据接收机(状态机)[D_pFun]。推荐使用线程，因为这样可以不进主线处理数据，这与MCU的中断处理逻辑不谋而合。
    4).线程任务[tcp_server_link_task]是维持socket关键，而且也是server的初始化，它是需要阻塞来等待连接的，连接之后这个线程还能执行接收函数[tcp_server_Receive_Get_Fun]或[D_pFun]
    5).配置/查询状态由[tcp_server_link_config]实现。
    ——2025/1/17 
*/

int tcp_server_link_config (char *port_str,int enable);
void *tcp_server_link_task(void *empty);
int tcp_server_send_data(uint8_t *data, int size);
void tcp_server_receive_State_Machine_Bind (D_pFun Callback_pFun);
int tcp_server_Receive_Get_Fun(uint8_t* data,int size);

#endif
