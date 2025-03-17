#ifndef _TCP_SERVER_LINK__H_
#define _TCP_SERVER_LINK__H_

#include "stdint.h"
#include "Network_manage.h"

/*

*/

int tcp_server_link_config (char *port_str,int enable);
void tcp_server_link_task(void *empty);
int tcp_server_send_data(uint8_t *data, int size);
void tcp_server_receive_State_Machine_Bind (D_Callback_pFun Callback_pFun);

#endif
