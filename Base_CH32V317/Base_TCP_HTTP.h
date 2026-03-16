#ifndef _BASE_TCP_HTTP__H_
#define _BASE_TCP_HTTP__H_

#if DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"


int Base_TCP_HTTP_Config (char *url,int enable);
int Base_TCP_HTTP_cache_Send_Fun (char *data, int len);
int Base_TCP_HTTP_cache_Read_Fun (char *data,int len_max);

void Base_TCP_HTTP_Task (u8 noway,u8 nobady);

#endif
