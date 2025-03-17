#ifndef _TCP_UDP_MULTICAST__H_
#define _TCP_UDP_MULTICAST__H_

#include "stdint.h"
#include "Network_manage.h"

int tcp_udp_multicast_Sd_config (char *ip_str,char *port_str,int enable);
int tcp_udp_multicast_Rv_config (char *ip_str,char *port_str,int enable);
void *tcp_udp_multicast_task(void *empty);
int tcp_udp_multicast_send_data(uint8_t *data, int size);
void tcp_udp_multicast_receive_State_Machine_Bind (D_pFun Callback_pFun);

#endif
