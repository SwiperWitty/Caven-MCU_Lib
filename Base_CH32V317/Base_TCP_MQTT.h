#ifndef _BASE_TCP_MQTT__H_
#define _BASE_TCP_MQTT__H_

#if DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"


int Base_TCP_Mqtt_Config (char *ip,char *port_str,char *user,char *passwd);
int Base_TCP_Mqtt_Pub (char *topic, int qos, uint8_t *data, int len);
void Base_TCP_Mqtt_Sub_Bind_Fun (char *topic, int qos, D_pFun Fun);

void Base_TCP_Mqtt_Task (u8 intstat);

#endif
