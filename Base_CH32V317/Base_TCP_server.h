#ifndef _BASE_TCP_SERVER__H_
#define _BASE_TCP_SERVER__H_

#if DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"


int Base_TCP_Server_Config (char *port_str,int enable);
int Base_TCP_Server_Send (uint8_t *data, int len);
void Base_TCP_Server_Receive_Bind_Fun (D_pFun Fun);

#endif
