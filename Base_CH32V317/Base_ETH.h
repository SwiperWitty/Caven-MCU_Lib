#ifndef _BASE_ETH__H_
#define _BASE_ETH__H_
/*
    occupy EXTI9_5_IRQHandler,TIM2_IRQHandler

    1).eth_init_flag,Base_ETH_Init over
    2).eth_flag,1:PHY ok,2:DHCP or static over
    3).p_eth_DNS_IP:by [Base_ETH_data_To_url]fun,HTTP,MQTT eth_Domain_name or URL TO ip_str...
        --eth_Domain_name:Base_ETH_Init start,Base_ETH_Task(eth_DNS_flag = 1) running,WCHNET_DNSCallBack over
    4).server_sock,client_sock:Base_ETH_Server_Bind,Base_ETH_Client_Bind.

    --2026.3.18
*/
#if DEFAULT
#include "Items.h"              // 
#else
#include "User_items.h"         // 
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

extern void Debug_OutStr(char *data);
extern int Debug_printf(const char *fmt, ...);

int Base_ETH_Init(int Channel,int Set);
void Base_ETH_Task (void);

void Base_ETH_get_MAC (uint8_t *mac);
void Base_ETH_set_MAC (uint8_t *mac);

int Base_ETH_get_status (void);
int Base_ETH_data_To_url (char *data,char *url,char *path);

//
u8 * Base_ETH_SockBuff_Bind (void);
u8 (*Base_ETH_SockBuff_Bind_RecBuf(void))[RECE_BUF_LEN];
u8 * Base_ETH_Server_Bind (void);
u8 * Base_ETH_Serlast_Bind (void);
u8 * Base_ETH_Client_Bind (void);

void Base_ETH_Server_pFun_Bind (ETH_handle_pFun fun);
void Base_ETH_Client_pFun_Bind (ETH_handle_pFun fun);
void Base_ETH_HTTP_pFun_Bind (ETH_handle_pFun fun);
void Base_ETH_MQTT_pFun_Bind (ETH_handle_pFun fun);
//

int Base_WIFI_config_local_ip (char mode,char *ip_str,char *gw_str,char *netmask_str);
int Base_ETH_config_local_ip (char mode,char *ip_str,char *gw_str,char *netmask_str);
int Base_ETH_config_local_DNS (char *DNS1,char *DNS2);

int Base_WIFI_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str);
int Base_ETH_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str);
int Base_ETH_get_SocketId (void);

#endif
