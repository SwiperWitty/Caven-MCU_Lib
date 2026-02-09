#ifndef _BASE_TCP_SERVER__H_
#define _BASE_TCP_SERVER__H_

#if DEFAULT
#include "Items.h"              // Ĭ�Ϲ���
#else
#include "User_items.h"         // �������ù��ܣ�һ������ڱ����ļ���User��
#endif

#include "Caven_Type.h"


int Base_TCP_Mqtt_Config (char *cfg, int enable);
int Base_TCP_Mqtt_Send (uint8_t *data, int len);
void Base_TCP_Mqtt_Sub_Bind_Fun (D_pFun Fun);


#endif
