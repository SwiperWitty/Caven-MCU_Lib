#ifndef _BASE_ETH__H_
#define _BASE_ETH__H_

#if DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"
#include "eth_driver.h"

typedef struct{
    int use_dhcp;
    char ip[16];
    char netmask[16];
    char gateway[16];
    char dns1[16];
    char dns2[16];
}net_cfg_Type;

typedef void (*ETH_handle_pFun) (u8 sock,u8 intstat);

int Base_ETH_Init(int Channel,int Set);
void Base_ETH_Task (void);
int Base_ETH_IPprot (char *url,char *ip,char *port);

void Base_ETH_get_MAC (uint8_t *mac);
void Base_ETH_set_MAC (uint8_t *mac);

int Base_ETH_get_status (void);
//
u8 * Base_ETH_SockBuff_Bind (void);
u8 (*Base_ETH_SockBuff_Bind_RecBuf(void))[RECE_BUF_LEN];
u8 * Base_ETH_Server_Bind (void);
u8 * Base_ETH_Client_Bind (void);

void Base_ETH_Server_pFun_Bind (ETH_handle_pFun fun);
void Base_ETH_Client_pFun_Bind (ETH_handle_pFun fun);
//

int Base_WIFI_config_local_ip (char mode,char *ip_str,char *gw_str,char *netmask_str);
int Base_ETH_config_local_ip (char mode,char *ip_str,char *gw_str,char *netmask_str);

int Base_WIFI_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str);
int Base_ETH_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str);
int Base_ETH_get_SocketId (void);

#endif
