#ifndef _BASE_TCP_CLIENT__H_
#define _BASE_TCP_CLIENT__H_

#if DEFAULT
#include "Items.h"              // Ĭ�Ϲ���
#else
#include "User_items.h"         // �������ù��ܣ�һ������ڱ����ļ���User��
#endif

#include "Caven_Type.h"


int Base_TCP_Client_Config (char *tcp_url, char *post_str,int enable);
int Base_TCP_Client_Send (uint8_t *data, int len);
void Base_TCP_Client_Receive_Bind_Fun (D_pFun Fun);



#endif
