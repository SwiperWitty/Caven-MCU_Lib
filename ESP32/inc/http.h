#ifndef _HTTP_H_
#define _HTTP_H_

#include "stdint.h"
#include "Network_manage.h"

/*

*/

int http_client_config_init (char *url_str,char *port_str,int enable);

void http_receive_Callback_Bind (D_Callback_pFun Callback_pFun);
int http_port_data_Fun (char *data);
void http_cache_port_data_Fun (char *data);
void http_cache_clean (void);

void http_cache_port_State_machine (void *empty);

#endif
