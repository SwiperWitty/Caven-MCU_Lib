#include "Base_ETH.h"
#include "API.h"
#include "eth_driver.h"

#if Exist_ETH

    #if WIFI_LINK
#define WIFI_SSID_SCAN_MAX_VALUE 20
static wifi_ap_record_t wifi_ap_info[WIFI_SSID_SCAN_MAX_VALUE];
static uint16_t wifi_ap_num = WIFI_SSID_SCAN_MAX_VALUE;
static char WIFI_name[30] = {0};
static char WIFI_pass[30] = {0};
// 这里是本次设备分配到的ip，很可能是动态的
static uint8_t s_WIFI_ip[4];
static uint8_t s_WIFI_netmask[4];
static uint8_t s_WIFI_gateway[4];

static char wifi_mode = 0;      // 0:static   1:dhcp
static char wifi_ip[30];
static char wifi_gw[30];
static char wifi_netmask[30];

static int wifi_flag = 0;               // 1:wifi建立连接
static int wifi_init_flag = 0;          // 1:初始化

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
// 静态的ip
static char eth_mode = 0;       // 0:static   1:dhcp
static char eth_ip[30];
static char eth_gw[30];
static char eth_netmask[30];
static char eth_DNS1_str[30];
static char eth_DNS2_str[30];

// 这里是本次设备分配到的ip，很可能是动态的
static uint8_t s_RJ45_ip[4];
static uint8_t s_RJ45_netmask[4];
static uint8_t s_RJ45_gateway[4];

static char s_eth_DNS1[4];
static char s_eth_DNS2[4];

static int eth_flag = 0;        // 1:rj45建立连接
static int eth_init_flag = 0;   // 1:初始化

static u8 SocketId;
static u8 server_sock = 0,client_sock = 0xff;
static ETH_handle_pFun ETH_Clinet_pFun = NULL;
static ETH_handle_pFun ETH_Server_pFun = NULL;
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

        eth_init_flag = 1;
        return READY;
    }
    else
    {
        eth_init_flag = 0;
        if(memcmp(s_RJ45_ip, tmp ,sizeof(s_RJ45_ip))){
            /*The obtained IP is different from the last value*/
            WCHNET_SocketClose(SocketId, TCP_CLOSE_NORMAL);
        }
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

void ETH_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void EXTI9_5_IRQHandler(void) __attribute__((interrupt()));

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
    url:192.168.1.168:8160
    ip:192.168.1.168
    port:8160
    retval:1

    url:192.168.1.168
    ip:192.168.1.168
    port:NULL
    retval:0
*/
int Base_ETH_IPprot (char *url,char *ip,char *port)
{
    int retval = -1;
    int temp_num = 0;
    char *str_pointer = NULL;
    if (url == NULL || ip == NULL || port == NULL)
    {
        return retval;
    }
    if(url[0] >= '0' && url[0] <= '9')
    {
        retval = 0;
        temp_num = strlen(url);
        str_pointer = memstr(url,":",temp_num);
        if(str_pointer != NULL)
        {
            temp_num = str_pointer-url;
            memcpy(ip,url,temp_num);
   
            strcpy(port,&url[temp_num+1]);
            retval = 1;
            // ESP_LOGI(TAG, "IPprot ip: [%s] port: [%s]", ip,port);
        }
        else
        {
            strcpy(ip,url);
        }
    }
    else
    {
        return retval;
    }
    return retval;
}

/*
    mode = 0, static
    mode = 1, dhcp
    mode = other    查询mode
    ip_str  ip地址
    gw_str  默认网关
    netmask_str 掩码
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
    ip_str = NULL,理解为查询网络状态
    retval = 1，在线
    retval = 0，掉线
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
    retval = 0,无网络
    retval = 1,wifi连接
    retval = 2,rj45连接
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
    if(client_sock == 0)
    {
        if(ETH_Clinet_pFun != NULL)
        {
            ETH_Clinet_pFun (0, 0);
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
            printf("GINT_STAT_UNREACH\r\n");
        }
        if (intstat & GINT_STAT_IP_CONFLI)                            //IP conflict
        {
            printf("GINT_STAT_IP_CONFLI\r\n");
        }

        if (intstat & GINT_STAT_PHY_CHANGE)                           //PHY status change
        {
            i = WCHNET_GetPHYStatus();
            if (i & PHY_Linked_Status)
            {
                printf("PHY Link Success\r\n");
                eth_flag = 1;
                client_sock = 0;
                server_sock = 0;
            }
            else
            {
                printf("PHY Link down\r\n");
                eth_flag = 0;
                if(server_sock)
                {
                    printf("eth link_sock %d \r\n",server_sock);
                    WCHNET_SocketClose(server_sock,TCP_CLOSE_ABANDON);
                    server_sock = 0;
                }
                if(client_sock)
                {
                    printf("eth link_sock %d \r\n",client_sock);
                    WCHNET_SocketClose(client_sock,TCP_CLOSE_ABANDON);
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
            }
            else
            {
                WCHNET_GetMacAddr(SYS_MAC_addr);
                TIM2_Init();
                if(eth_mode == 1)
                {
                    WCHNET_DHCPSetHostname("CAVEN_3.14 ETH");
                    memset(s_RJ45_ip,0,sizeof(s_RJ45_ip));
                    memset(s_RJ45_gateway,0,sizeof(s_RJ45_gateway));
                    memset(s_RJ45_netmask,0,sizeof(s_RJ45_netmask));
                    retval = ETH_LibInit(s_RJ45_ip, s_RJ45_gateway, s_RJ45_netmask, SYS_MAC_addr);
                    if(retval == WCHNET_ERR_SUCCESS)
                    {
                        WCHNET_DHCPStart(WCHNET_DHCPCallBack);  // Start DHCP
                        eth_init_flag = 1;
                        retval = 1;
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
                    Caven_Str_To_ip (eth_ip,s_RJ45_ip,4);
                    Caven_Str_To_ip (eth_gw,s_RJ45_gateway,4);
                    Caven_Str_To_ip (eth_netmask,s_RJ45_netmask,4);
                    retval = ETH_LibInit(s_RJ45_ip, s_RJ45_gateway, s_RJ45_netmask, SYS_MAC_addr);
                    if(retval == WCHNET_ERR_SUCCESS)
                    {
                        eth_init_flag = 1;
                        retval = 1;
                    }
                    struct _KEEP_CFG cfg;

                    cfg.KLIdle = 20000;
                    cfg.KLIntvl = 15000;
                    cfg.KLCount = 9;
                    WCHNET_ConfigKeepLive(&cfg);
                }
            }
            printf("WCH ETH IP %s,GW %s,MK %s \r\n",eth_ip,eth_gw,eth_netmask);
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
