#ifndef _TCP_CLIENT_LINK__H_
#define _TCP_CLIENT_LINK__H_

#include "stdint.h"
#include "Network_manage.h"

/*

*/

int tcp_client_link_config (char *ip_str,char *port_str,int enable);
void tcp_client_link_task(void *empty);
int tcp_client_send_data(uint8_t *data, int size);
void tcp_client_receive_State_Machine_Bind (D_Callback_pFun Callback_pFun);

#endif
