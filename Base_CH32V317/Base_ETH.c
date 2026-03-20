#include "Base_ETH.h"
#include "API.h"
#include "eth_driver.h"

#if Exist_ETH

void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt()));

    #if WIFI_LINK
#define WIFI_SSID_SCAN_MAX_VALUE 20
static wifi_ap_record_t wifi_ap_info[WIFI_SSID_SCAN_MAX_VALUE];
static uint16_t wifi_ap_num = WIFI_SSID_SCAN_MAX_VALUE;
static char WIFI_name[30] = {0};
static char WIFI_pass[30] = {0};
// �����Ǳ����豸���䵽��ip���ܿ����Ƕ�̬��
static uint8_t s_WIFI_ip[4];
static uint8_t s_WIFI_netmask[4];
static uint8_t s_WIFI_gateway[4];

static char wifi_mode = 0;      // 0:static   1:dhcp
static char wifi_ip[30];
static char wifi_gw[30];
static char wifi_netmask[30];

static int wifi_flag = 0;               // 1:wifi��������
static int wifi_init_flag = 0;          // 1:��ʼ��

int Base_WIFI_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str)
{
    int retval = 0;
    if (ip_str == NULL || gw_str == NULL || netmask_str == NULL)
    {
    }
    else
    {
        memcpy(ip_str,s_WIFI_ip,4);
        memcpy(gw_str,s_WIFI_netmask,4);
        memcpy(netmask_str,s_WIFI_gateway,4);
    }
    retval = wifi_flag;
    return retval;
}
    #endif

static uint8_t SYS_MAC_addr[6] = {0};
#define ETH_SOCK_NUM    2
// 
static char eth_mode = 0;       // 0:static   1:dhcp
static char eth_ip[30];
static char eth_gw[30];
static char eth_netmask[30];
static char eth_DNS1_str[30];
static char eth_DNS2_str[30];
static char eth_Domain_name[200];
static char *p_eth_DNS_IP = NULL;
static char eth_url_port[20];
static char eth_DNS_flag = 0;
//
static uint8_t s_RJ45_ip[4];
static uint8_t s_RJ45_netmask[4];
static uint8_t s_RJ45_gateway[4];
static uint8_t s_eth_DNS1[4];
static uint8_t s_eth_DNS2[4];

static int eth_flag = 0;        //
static int eth_init_flag = 0;   //

u16 DnsPort = 53;
static u8 SocketId;
__attribute__((__aligned__(4))) u8  RemoteIp[4];
u8 server_sock = 0xff,serlast_sock = 0xff,client_sock = 0xff;
static ETH_handle_pFun ETH_Clinet_pFun = NULL;
static ETH_handle_pFun ETH_Server_pFun = NULL;
static ETH_handle_pFun ETH_HTTP_pFun = NULL;
static ETH_handle_pFun ETH_MQTT_pFun = NULL;
static u8 base_socket[WCHNET_MAX_SOCKET_NUM];                //Save the currently connected socket
static u8 base_SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  // socket receive buffer

u8 * Base_ETH_SockBuff_Bind (void)
{
    return base_socket;
}

u8 (*Base_ETH_SockBuff_Bind_RecBuf(void))[RECE_BUF_LEN] 
{
    return base_SocketRecvBuf;
}

u8 * Base_ETH_Server_Bind (void)
{
    return &server_sock;
}

u8 * Base_ETH_Serlast_Bind (void)
{
    return &serlast_sock;
}

u8 * Base_ETH_Client_Bind (void)
{
    return &client_sock;
}

void Base_ETH_Server_pFun_Bind (ETH_handle_pFun fun)
{
    ETH_Server_pFun = fun;
}

void Base_ETH_Client_pFun_Bind (ETH_handle_pFun fun)
{
    ETH_Clinet_pFun = fun;
}

void Base_ETH_HTTP_pFun_Bind (ETH_handle_pFun fun)
{
    ETH_HTTP_pFun = fun;
}

void Base_ETH_MQTT_pFun_Bind (ETH_handle_pFun fun)
{
    ETH_MQTT_pFun = fun;
}

void WCHNET_DNSCallBack(const char *name, u8 *ipaddr, void *callback_arg)
{
    if(p_eth_DNS_IP == NULL)
    {
        return;
    }
    memset(p_eth_DNS_IP,0,2);
    Debug_printf("Host Name:[%s]\r\n", name);
    if(ipaddr == NULL)
    {
        Debug_printf("DNS Fail\r\n");
        return;
    }
    sprintf(p_eth_DNS_IP,"%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    if(strlen(eth_url_port) > 0)
    {
        strcat(p_eth_DNS_IP,":");
        strcat(p_eth_DNS_IP,eth_url_port);
    }
    Debug_printf("url:[%s]\r\n", p_eth_DNS_IP);
    if(callback_arg != NULL)
    {
        Debug_printf("callback_arg: %02x\r\n", (*(u8 *)callback_arg));
    }
    WCHNET_DNSStop();                                                          //stop DNS,and release socket
}

/* 测试项：
    http://192.168.1.128:9090/loci
    http://192.168.1.128
    http://192.168.1.128:9090
    tcp://dianjixz.online:1883
    http://example.com
    
    url是全局参数
*/
int Base_ETH_data_To_url (char *data,char *g_url,char *path)
{
    int retval = 0;
    int http_len = 0,temp_num = 0;
    char array_url[200],array_port[20];
    char *p_temp = NULL,*p_temp_port = NULL,*p_temp_path = NULL;
    
    if (data != NULL && g_url != NULL && path != NULL)
    {
        g_url[0] = 0;
        path[0] = 0;
        memset(array_url,0,sizeof(array_url));
        memset(array_port,0,sizeof(array_port));
        memset(eth_Domain_name,0,sizeof(eth_Domain_name));
        p_eth_DNS_IP = g_url;
        http_len = strlen(data);
        p_temp = memstr(data, "://",http_len);
        if (p_temp != NULL)
        {
            p_temp = p_temp + strlen("://");
            http_len = http_len - (p_temp - data);

            p_temp_port = memstr(p_temp, ":",http_len); // port
            if (p_temp_port != NULL)
            {
                retval = p_temp_port - p_temp;
                memcpy(array_url,p_temp,retval);
                strcat(eth_Domain_name,array_url);
                p_temp_port ++;
                temp_num = atoi(p_temp_port);
                if(temp_num)
                {
                    sprintf(eth_url_port,"%d",temp_num);
                    strcat(array_url,":");
                    strcat(array_url,eth_url_port);
                }
            }
            else
            {
                memset(eth_url_port,0,sizeof(eth_url_port));
                p_temp_path = memstr(p_temp, "/",http_len); // path
                if(p_temp_path == NULL)
                {
                    retval = http_len;
                    memcpy(array_url,p_temp,retval);
                    strcat(eth_Domain_name,array_url);
                }
                else
                {
                    http_len = p_temp_path - p_temp;
                    retval = http_len;
                    memcpy(array_url,p_temp,retval);
                    strcat(eth_Domain_name,array_url);
                    p_temp_path += 1;
                    strcpy(path,p_temp_path);
                }
            }
            p_temp_path = memstr(p_temp, "/",http_len);
            if (p_temp_path != NULL)
            {
                p_temp_path += 1;
                strcpy(path,p_temp_path);
            }
            Debug_printf("data_To_url :%s \n",array_url);
            Debug_printf("data_To_path :%s \n",path);
            if (p_temp[0] < '0' || p_temp[0] > '9')    // Domain name
            {
                eth_DNS_flag = 1;       // Asynchronous
                retval = 0;
            }
            else
            {
                strcat(p_eth_DNS_IP,array_url);
            }
        }
    }
    return retval;      // 大于0即成功解析
}

static u8 WCHNET_DHCPCallBack(u8 status, void *arg)
{
    u8 *p;
    u8 tmp[4] = {0, 0, 0, 0};

    if(!status)
    {
        p = arg;
        
        /*If the obtained IP is the same as the last IP, exit this function.*/
        if(!memcmp(s_RJ45_ip, p ,sizeof(s_RJ45_ip)))
            return READY;
        /*Determine whether it is the first successful IP acquisition*/
        if(memcmp(s_RJ45_ip, tmp ,sizeof(s_RJ45_ip))){
            /*The obtained IP is different from the last value,
             * then disconnect the last connection.*/
            WCHNET_SocketClose(SocketId, TCP_CLOSE_NORMAL);
        }
        
        memcpy(s_RJ45_ip, p, 4);
        memcpy(s_RJ45_gateway, &p[4], 4);
        memcpy(s_RJ45_netmask, &p[8], 4);
        memcpy(s_eth_DNS1, &p[12], 4);
        memcpy(s_eth_DNS2, &p[16], 4);
        WCHNET_InitDNS(s_eth_DNS1, DnsPort);    // DHCP DNS
        //
        memset(eth_ip,0,sizeof((eth_ip)));
        memset(eth_gw,0,sizeof((eth_gw)));
        memset(eth_netmask,0,sizeof((eth_netmask)));
        memset(eth_DNS1_str,0,sizeof((eth_DNS1_str)));
        memset(eth_DNS2_str,0,sizeof((eth_DNS2_str)));
        sprintf(eth_ip,"%d.%d.%d.%d",s_RJ45_ip[0],s_RJ45_ip[1],s_RJ45_ip[2],s_RJ45_ip[3]);
        sprintf(eth_gw,"%d.%d.%d.%d",s_RJ45_gateway[0],s_RJ45_gateway[1],s_RJ45_gateway[2],s_RJ45_gateway[3]);
        sprintf(eth_netmask,"%d.%d.%d.%d",s_RJ45_netmask[0],s_RJ45_netmask[1],s_RJ45_netmask[2],s_RJ45_netmask[3]);
        sprintf(eth_DNS1_str,"%d.%d.%d.%d",s_eth_DNS1[0],s_eth_DNS1[1],s_eth_DNS1[2],s_eth_DNS1[3]);
        sprintf(eth_DNS2_str,"%d.%d.%d.%d",s_eth_DNS2[0],s_eth_DNS2[1],s_eth_DNS2[2],s_eth_DNS2[3]);

        Debug_printf("WCH ETH DHCP OK,IP %s,GW %s,MK %s \n DNS:%s\r\n",eth_ip,eth_gw,eth_netmask,eth_DNS1_str);
        eth_flag = 2;
        return READY;
    }
    else
    {
        Debug_OutStr("DHCP Fail \r\n");
        if(memcmp(s_RJ45_ip, tmp ,sizeof(s_RJ45_ip))){
            /*The obtained IP is different from the last value*/
            WCHNET_SocketClose(SocketId, TCP_CLOSE_NORMAL);
        }
        eth_init_flag = 2;
        return NoREADY;
    }
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   This function handles TIM2 exception.
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{
    WCHNET_TimeIsr(WCHNETTIMERPERIOD);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

/*********************************************************************
 * @fn      EXTI9_5_IRQHandler
 *
 * @brief   This function handles GPIO exception.
 *
 * @return  none
 */
void EXTI9_5_IRQHandler(void)
{
    ETH_PHYLink( );
    EXTI_ClearITPendingBit(EXTI_Line7);     /* Clear Flag */
}

/*********************************************************************
 * @fn      ETH_IRQHandler
 *
 * @brief   This function handles ETH exception.
 *
 * @return  none
 */
void ETH_IRQHandler(void)
{
    WCHNET_ETHIsr();
}

#endif

/*
    mode = 0, static
    mode = 1, dhcp
    mode = other    ��ѯmode
    ip_str  ip��ַ
    gw_str  Ĭ������
    netmask_str ����
*/
int Base_ETH_config_local_ip (char mode,char *ip_str,char *gw_str,char *netmask_str)
{
    int retval = 0;
#if Exist_ETH
    if (mode == 0)
    {
        eth_mode = 0;
    }
    else if (mode == 1)
    {
        eth_mode = 1;
    }
    else
    {
        retval = eth_mode;
        return retval;
    }
    if (ip_str != NULL)
    {
        memset(eth_ip,0,sizeof(eth_ip));
        memset(eth_gw,0,sizeof(eth_gw));
        memset(eth_netmask,0,sizeof(eth_netmask));
        
        memcpy(eth_ip,ip_str,strlen(ip_str));
        memcpy(eth_gw,gw_str,strlen(gw_str));
        memcpy(eth_netmask,netmask_str,strlen(netmask_str));
    }
#endif
    return retval;
}

int Base_ETH_config_local_DNS (char *DNS1,char *DNS2)
{
    int retval = 0;
#if Exist_ETH
    if (DNS1 == NULL || DNS2 == NULL)
    {
        return retval;
    }
    else
    {
        memset(eth_DNS1_str,0,sizeof(eth_DNS1_str));
        memset(eth_DNS2_str,0,sizeof(eth_DNS2_str));
        strcpy(eth_DNS1_str,DNS1);
        strcpy(eth_DNS2_str,DNS2);
    }
    retval = 1;
#endif
    return retval;
}

/*
    ip_str = NULL,return net state
    retval = 0,
    retval = 1,PHY Link Success
    retval = 2,Static/DHCP OK
*/
int Base_ETH_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str)
{
    int retval = 0;
#if Exist_ETH
    if (ip_str == NULL || gw_str == NULL || netmask_str == NULL)
    {
    }
    else
    {
        memcpy(ip_str,s_RJ45_ip,4);
        memcpy(gw_str,s_RJ45_gateway,4);
        memcpy(netmask_str,s_RJ45_netmask,4);
    }
    retval = eth_flag;
#endif
    return retval;
}

int Base_ETH_get_local_DNS (uint8_t *DNS1,uint8_t *DNS2)
{
    int retval = 0;
#if Exist_ETH
    if (DNS1 == NULL || DNS2 == NULL)
    {
    }
    else
    {
        strcpy((char*)DNS1,eth_DNS1_str);
        strcpy((char*)DNS2,eth_DNS2_str);
    }
    retval = 1;
#endif
    return retval;
}

void Base_ETH_set_MAC (uint8_t *mac)
{
#if Exist_ETH
    if (mac != NULL)
    {
        memcpy(SYS_MAC_addr,mac,6);
    }
#endif
}

void Base_ETH_get_MAC (uint8_t *mac)
{
#if Exist_ETH
    if (mac != NULL)
    {
        memcpy(mac,SYS_MAC_addr,6);
    }
#endif
}

/*
    retval = 0,no net
    retval = 1,wifi PHY
    retval = 2,rj45 PHY
*/
int Base_ETH_get_status (void)
{
    int retval = 0,temp = 0;
#if WIFI_LINK
    if (Base_WIFI_get_local_ip_status(NULL,NULL,NULL))
    {
        retval |= 0x01 << 0;
    }
    else
    {
        temp = 0x01 << 0;
        retval &= ~temp;
    }
#endif
    if (Base_ETH_get_local_ip_status(NULL,NULL,NULL))
    {
        retval |= 0x01 << 1;
    }
    else
    {
        temp = 0x01 << 1;
        retval &= ~temp;
    }
    return retval;
}

int Base_ETH_get_SocketId (void)
{
    int retval = 0;
#if Exist_ETH
    retval = SocketId;
#endif
    return retval;
}

void Base_ETH_Task (void)
{
#if Exist_ETH
    if(eth_init_flag == 0)
    {
        return;
    }
    /*Ethernet library main task function,
        * which needs to be called cyclically*/
    WCHNET_MainTask();
    /*Query the Ethernet global interrupt,
        * if there is an interrupt, call the global interrupt handler*/
    if(eth_flag == 2)        // net init
    {
        if(eth_DNS_flag)
        {
            eth_DNS_flag = 0;
            WCHNET_HostNameGetIp(eth_Domain_name, RemoteIp, WCHNET_DNSCallBack, NULL);  // Start DNS
        }
        if(ETH_Clinet_pFun != NULL)
        {
            ETH_Clinet_pFun (0, 0);
        }
        if(ETH_Server_pFun != NULL)
        {
            ETH_Server_pFun (0, 0);
        }
        if(ETH_HTTP_pFun != NULL)
        {
            ETH_HTTP_pFun (0, 0);
        }
        if(ETH_MQTT_pFun != NULL)
        {
            ETH_MQTT_pFun (0, 0);
        }
    }
    if(WCHNET_QueryGlobalInt())
    {
        u8 intstat;
        u16 i;
        u8 socketint;
        intstat = WCHNET_GetGlobalInt();                              //get global interrupt flag
        if (intstat & GINT_STAT_UNREACH)                              //Unreachable interrupt
        {
            Debug_OutStr("GINT_STAT_UNREACH\r\n");
        }
        if (intstat & GINT_STAT_IP_CONFLI)                            //IP conflict
        {
            Debug_OutStr("GINT_STAT_IP_CONFLI\r\n");
        }

        if (intstat & GINT_STAT_PHY_CHANGE)                           //PHY status change
        {
            i = WCHNET_GetPHYStatus();
            if (i & PHY_Linked_Status)
            {
                Debug_OutStr("PHY Link Success\r\n");
                eth_init_flag = 1;
                if(eth_flag == 0)    // Reply
                {
                    eth_flag = 2;
                    Debug_OutStr("PHY Link again\r\n");
                }
                if(eth_mode == 0)       // static
                {
                    eth_flag = 2;
                    if(strlen(eth_DNS1_str) > 0)
                    {
                        Caven_Str_To_ip (eth_DNS1_str,s_eth_DNS1,4);
                        WCHNET_InitDNS(s_eth_DNS1, DnsPort);
                    }
                }
                client_sock = 0xff;
                server_sock = 0xff;
                serlast_sock = 0xff;
            }
            else
            {
                Debug_OutStr("PHY Link down\r\n");
                eth_flag = 0;
                if(server_sock != 0xff)
                {
                    WCHNET_SocketClose(server_sock,TCP_CLOSE_RST);
                    server_sock = 0xff;
                    serlast_sock = 0xff;
                }
                if(client_sock != 0xff)
                {
                    WCHNET_SocketClose(client_sock,TCP_CLOSE_RST);
                    client_sock = 0xff;
                }
            }
        }
        if (intstat & GINT_STAT_SOCKET) {                             //socket related interru
            for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) 
            {
                socketint = WCHNET_GetSocketInt(i);
                if (socketint)
                {
                    if(i == client_sock)
                    {
                        if(ETH_Clinet_pFun != NULL)
                        {
                            ETH_Clinet_pFun (i, socketint);
                        }
                    }
                    else
                    {
                        if(ETH_Server_pFun != NULL)
                        {
                            ETH_Server_pFun (i, socketint);
                        }
                    }
                }
            }
        }
    }
#endif
}

int Base_ETH_Init(int Channel,int Set)
{
    int retval = 0;
#if Exist_ETH
    if (Channel == 1) {
    #if WIFI_LINK
        if(Set)
        {
            retval = wifi_init_flag;
        }
        else
        {
            retval = 1;
        }
    #endif
    }
    else if (Channel == 2) {
        if(Set)
        {
            memset(base_socket, 0xff, WCHNET_MAX_SOCKET_NUM);
            if (WCHNET_LIB_VER != WCHNET_GetVer()) {
                eth_init_flag = 0;
                return 0;
            }
            else
            {
                WCHNET_GetMacAddr(SYS_MAC_addr);

                TIM2_Init();
                if(eth_mode == 1)
                {
                    WCHNET_DHCPSetHostname("WCHNET");
                    memset(s_RJ45_ip,0,sizeof(s_RJ45_ip));
                    memset(s_RJ45_gateway,0,sizeof(s_RJ45_gateway));
                    memset(s_RJ45_netmask,0,sizeof(s_RJ45_netmask));
                    retval = ETH_LibInit(s_RJ45_ip, s_RJ45_gateway, s_RJ45_netmask, SYS_MAC_addr);
                    if(retval == WCHNET_ERR_SUCCESS)
                    {
                        eth_init_flag = 1;
                        eth_flag = 1;
                        Debug_OutStr("WCH ETH WCHNET_ERR_SUCCESS,DHCP IP Start ...\r\n");
                        WCHNET_DHCPStart(WCHNET_DHCPCallBack);  // Start DHCP
                        retval = 1;
                    }
                    else
                    {
                        Debug_OutStr("WCH ETH DHCP IP error\r\n");
                        eth_init_flag = 2;
                        eth_flag = 1;
                    }
                }
                else
                {
                    if(strlen(eth_ip) == 0)
                    {
                        strcpy(eth_ip,"192.168.1.168");
                        strcpy(eth_gw,"192.168.1.1");
                        strcpy(eth_netmask,"255, 255, 255, 0");
                    }
                    if (strlen(eth_DNS1_str) == 0)
                    {
                        strcpy(eth_DNS1_str,"114.114.114.114");
                        strcpy(eth_DNS2_str,"8.8.8.8");
                    }
                    Caven_Str_To_ip (eth_ip,s_RJ45_ip,4);
                    Caven_Str_To_ip (eth_gw,s_RJ45_gateway,4);
                    Caven_Str_To_ip (eth_netmask,s_RJ45_netmask,4);
                    Caven_Str_To_ip (eth_DNS1_str,s_eth_DNS1,4);
                    Caven_Str_To_ip (eth_DNS2_str,s_eth_DNS2,4);
                    retval = ETH_LibInit(s_RJ45_ip, s_RJ45_gateway, s_RJ45_netmask, SYS_MAC_addr);
                    eth_flag = 1;
                    if(retval == WCHNET_ERR_SUCCESS)
                    {
                        Debug_OutStr("WCH ETH WCHNET_ERR_SUCCESS \r\n");
                        eth_init_flag = 1;
                        retval = 1;
                    }
                    struct _KEEP_CFG cfg;

                    cfg.KLIdle = 20000;
                    cfg.KLIntvl = 15000;
                    cfg.KLCount = 9;
                    WCHNET_ConfigKeepLive(&cfg);
                    Debug_printf("WCH ETH static IP %s,GW %s,MK %s \r\n",eth_ip,eth_gw,eth_netmask);
                }
            }
        }
        else 
        {
            retval = 1;
        }
    }
    else {
    
    }
#endif
    return retval;
}
