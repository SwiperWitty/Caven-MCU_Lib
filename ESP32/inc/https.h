#ifndef _HTTPS_H_
#define _HTTPS_H_

#include "stdint.h"
#include "Network_manage.h"

#include "http.h"

/*

*/
#define WEB_SERVER "https://open-sandbox.atma.io"
#define WEB_PORT "443"
#define WEB_URL "https://open-sandbox.atma.io/trace/track-and-trace/api/v1/rovinj/events?tenant=rovinj"

int https_request_config_init (char *way_str,char *Host_str,char *URL_str,char *port_str,int enable);

int https_request_add_header (char *type,char *data);

void https_receive_Callback_Bind (D_Callback_pFun Callback_pFun);
int https_request_data_Fun (char type,char *data);
void https_cache_request_data_Fun (char type,char *data);
void https_cache_clean (void);

void https_cache_request_State_machine (void *empty);

void eps32_HTTPS_task (void *empty);

#endif
