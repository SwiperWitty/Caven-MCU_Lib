#ifndef _BASE_ETH__H_
#define _BASE_ETH__H_

#if DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"

#define DEFAULT_WIFI_NAME "Cavendish"   // Cavendish
#define DEFAULT_WIFI_PASS "1234567890"  // 

typedef void (*D_Callback_pFun) (void *data);   // 数据回调类型

int Base_ETH_Init(int Channel,int Set);
void Base_ETH_Task (void);
int Base_ETH_Task_CallBcak_Bind(V_pFun fun);

void Base_ETH_get_MAC (uint8_t *mac);
void Base_ETH_set_MAC (uint8_t *mac);

int Base_ETH_get_status (void);

int Base_WIFI_config_local_ip (char mode,char *ip_str,char *gw_str,char *netmask_str);
int Base_ETH_config_local_ip (char mode,char *ip_str,char *gw_str,char *netmask_str);

int Base_WIFI_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str);
int Base_ETH_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str);
int Base_ETH_get_SocketId (void);

#endif
