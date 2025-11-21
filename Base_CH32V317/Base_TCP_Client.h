#ifndef _BASE_TCP_CLIENT__H_
#define _BASE_TCP_CLIENT__H_

#if DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"


int Base_TCP_Client_Config (char *ip,char *port_str,int enable);
int Base_TCP_Client_Send (uint8_t *data, int len);
void Base_TCP_Client_Receive_Bind_Fun (D_pFun Fun);

void Base_TCP_Client_Task (u8 sock,u8 intstat);

#endif
