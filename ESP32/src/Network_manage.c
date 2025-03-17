#include "Network_manage.h"

#include <stdio.h>
#include <string.h>
#include "stdlib.h"

#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_eth.h"

#include "nvs_flash.h"
#include "driver/gpio.h"
#include "freertos/event_groups.h"

#if CONFIG_ETH_USE_SPI_ETHERNET
#include "driver/spi_master.h"
#endif

uint16_t tcp_server_port_connect_counter_when_using; //!!!! 
uint16_t tcp_server_port;                            //!!!! 
uint8_t lwip_task_idle_timer_counter;                //!!!! 
uint8_t port_8160_using;
static const char *TAG = "Network_manage";

static int esp_netif_init_flag = 0;     // 1:初始化
static int wifi_flag = 0;               // 1:wifi建立连接
// 静态的ip
static uint8_t SYS_MAC_addr[6] = {0};
static char wifi_mode = 0;      // 0:dhcp   1:static
static char wifi_ip[30];
static char wifi_gw[30];
static char wifi_netmask[30];

#define CONFIG_ESP_WIFI_AUTH_OPEN 1
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN

#define WIFI_SSID_SCAN_MAX_VALUE 20
static wifi_ap_record_t wifi_ap_info[WIFI_SSID_SCAN_MAX_VALUE];
static uint16_t wifi_ap_num = WIFI_SSID_SCAN_MAX_VALUE;
static char WIFI_name[30] = {0};
static char WIFI_pass[30] = {0};
// 这里是本次设备分配到的ip，很可能是动态的
static uint8_t s_WIFI_ip[4];
static uint8_t s_WIFI_netmask[4];
static uint8_t s_WIFI_gateway[4];
static void print_auth_mode(int authmode)
{
    switch (authmode) {
    case WIFI_AUTH_OPEN:
        ESP_LOGI("wifi scan", "Authmode \tWIFI_AUTH_OPEN");
        break;
    case WIFI_AUTH_WEP:
        ESP_LOGI("wifi scan", "Authmode \tWIFI_AUTH_WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        ESP_LOGI("wifi scan", "Authmode \tWIFI_AUTH_WPA_PSK");
        break;
    case WIFI_AUTH_WPA2_PSK:
        ESP_LOGI("wifi scan", "Authmode \tWIFI_AUTH_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        ESP_LOGI("wifi scan", "Authmode \tWIFI_AUTH_WPA_WPA2_PSK");
        break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
        ESP_LOGI("wifi scan", "Authmode \tWIFI_AUTH_WPA2_ENTERPRISE");
        break;
    case WIFI_AUTH_WPA3_PSK:
        ESP_LOGI("wifi scan", "Authmode \tWIFI_AUTH_WPA3_PSK");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        ESP_LOGI("wifi scan", "Authmode \tWIFI_AUTH_WPA2_WPA3_PSK");
        break;
    default:
        ESP_LOGI("wifi scan", "Authmode \tWIFI_AUTH_UNKNOWN");
        break;
    }
}

static void print_cipher_type(int pairwise_cipher, int group_cipher)
{
    switch (pairwise_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI("wifi scan", "Pairwise Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI("wifi scan", "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI("wifi scan", "Pairwise Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI("wifi scan", "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI("wifi scan", "Pairwise Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI("wifi scan", "Pairwise Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    default:
        ESP_LOGI("wifi scan", "Pairwise Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }

    switch (group_cipher) {
    case WIFI_CIPHER_TYPE_NONE:
        ESP_LOGI("wifi scan", "Group Cipher \tWIFI_CIPHER_TYPE_NONE");
        break;
    case WIFI_CIPHER_TYPE_WEP40:
        ESP_LOGI("wifi scan", "Group Cipher \tWIFI_CIPHER_TYPE_WEP40");
        break;
    case WIFI_CIPHER_TYPE_WEP104:
        ESP_LOGI("wifi scan", "Group Cipher \tWIFI_CIPHER_TYPE_WEP104");
        break;
    case WIFI_CIPHER_TYPE_TKIP:
        ESP_LOGI("wifi scan", "Group Cipher \tWIFI_CIPHER_TYPE_TKIP");
        break;
    case WIFI_CIPHER_TYPE_CCMP:
        ESP_LOGI("wifi scan", "Group Cipher \tWIFI_CIPHER_TYPE_CCMP");
        break;
    case WIFI_CIPHER_TYPE_TKIP_CCMP:
        ESP_LOGI("wifi scan", "Group Cipher \tWIFI_CIPHER_TYPE_TKIP_CCMP");
        break;
    default:
        ESP_LOGI("wifi scan", "Group Cipher \tWIFI_CIPHER_TYPE_UNKNOWN");
        break;
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:      // WIFI以STA模式启动后触发此事件
            esp_wifi_connect();         // 启动WIFI连接
            break;
        case WIFI_EVENT_STA_CONNECTED:  // WIFI连上路由器后，触发此事件
            ESP_LOGI(TAG, "WIFI connected to AP succ");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:   // WIFI从路由器断开连接后触发此事件
            esp_wifi_connect();             // 继续重连
            ESP_LOGI(TAG,"WIFI connect to the AP fail,retry now[%s]",WIFI_name);
            break;
        default:
            break;
        }
        wifi_flag = 0;
        memset(s_WIFI_ip,0,sizeof(s_WIFI_ip));
        memset(s_WIFI_netmask,0,sizeof(s_WIFI_netmask));
        memset(s_WIFI_gateway,0,sizeof(s_WIFI_gateway));
    }
    if(event_base == IP_EVENT)                  // IP相关事件
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        switch(event_id)
        {
            case IP_EVENT_STA_GOT_IP:           // 只有获取到路由器分配的IP，才认为是连上了路由器
            {
                s_WIFI_ip[0] = esp_ip4_addr1_16(&event->ip_info.ip);
                s_WIFI_ip[1] = esp_ip4_addr2_16(&event->ip_info.ip);
                s_WIFI_ip[2] = esp_ip4_addr3_16(&event->ip_info.ip);
                s_WIFI_ip[3] = esp_ip4_addr4_16(&event->ip_info.ip);

                s_WIFI_netmask[0] = esp_ip4_addr1_16(&event->ip_info.netmask);
                s_WIFI_netmask[1] = esp_ip4_addr2_16(&event->ip_info.netmask);
                s_WIFI_netmask[2] = esp_ip4_addr3_16(&event->ip_info.netmask);
                s_WIFI_netmask[3] = esp_ip4_addr4_16(&event->ip_info.netmask);

                s_WIFI_gateway[0] = esp_ip4_addr1_16(&event->ip_info.gw);
                s_WIFI_gateway[1] = esp_ip4_addr2_16(&event->ip_info.gw);
                s_WIFI_gateway[2] = esp_ip4_addr3_16(&event->ip_info.gw);
                s_WIFI_gateway[3] = esp_ip4_addr4_16(&event->ip_info.gw);
                
                ESP_LOGI(TAG, "WIFIIP:" IPSTR, IP2STR(&event->ip_info.ip));
                ESP_LOGI(TAG, "WIFIMASK:" IPSTR, IP2STR(&event->ip_info.netmask));
                ESP_LOGI(TAG, "WIFIGW:" IPSTR, IP2STR(&event->ip_info.gw));
                ESP_LOGW(TAG, "WIFI Got IP Address \n");
                wifi_flag = 1;
            }
            break;
            default:
                break;
        }
    }
}

static int ipstr_to_numeric(const char *_str, uint32_t *_addr)
{
    const char *index;
    unsigned char *addr = (unsigned char *)_addr;
    int isnumeric = 1;
    int i = 3;
    *_addr = 0;
    index = _str;
    while ((*index) && (isnumeric))
    {
        if (isdigit((unsigned char)*index))
        {
            addr[i] = addr[i] * 10 + (*index - '0'); // big-endian
        }
        else if (*index == '.')
        {
            i--;
            if (i == -1)
                isnumeric = 0;
        }
        else
        {
            isnumeric = 0;
        }
        index++;
    }
    if (isnumeric && i)
        return -1; // error
    if (isnumeric)
        return 0; // successful
    else
        return -1;
}

static void get_ip_address_info(uint32_t ipadd, uint8_t *ip_buffer)
{
    ip_buffer[0] = ipadd >> 24;
    ipadd = ipadd << 8;
    ip_buffer[1] = ipadd >> 24;
    ipadd = ipadd << 8;
    ip_buffer[2] = ipadd >> 24;
    ipadd = ipadd << 8;
    ip_buffer[3] = ipadd >> 24;
}

void ipstr_to_ip_address(char *str, uint8_t *ip)
{
    uint32_t ip_add = 0;
    if (0 == ipstr_to_numeric(str, &ip_add))
    {
        get_ip_address_info(ip_add, ip);
    }
}
/*
    mode = 0,dhcp
    mode = 1,static (此时如果wifi_ip内数据有效则变更ip,也就是固定IP更新)
    mode = 2,set ip (不会立即生效，只会把内容放进ip设置里面，这样没有启动wifi_init,也能改ip)
    mode = other    查询mode
    ip_str  ip地址
    gw_str  默认网关
    netmask_str 掩码
*/
int wifi_config_ip (char mode,char *ip_str,char *gw_str,char *netmask_str)
{
    int retval = 0;
    if (mode == 0)
    {
        wifi_mode = 0;
        ESP_LOGW(TAG,"set wifi allocation dhcp ip");
    }
    else if (mode == 1)
    {
        wifi_mode = 1;
        ESP_LOGW(TAG,"set wifi allocation static ip");
    }
    else if (mode == 2)
    {
        ESP_LOGI(TAG,"set wifi ip");
    }
    else
    {
        retval = wifi_mode;
        return retval;
    }
    if (ip_str != NULL)
    {
        memset(wifi_ip,0,sizeof(wifi_ip));
        memset(wifi_gw,0,sizeof(wifi_gw));
        memset(wifi_netmask,0,sizeof(wifi_netmask));
        
        memcpy(wifi_ip,ip_str,strlen(ip_str));
        memcpy(wifi_gw,gw_str,strlen(gw_str));
        memcpy(wifi_netmask,netmask_str,strlen(netmask_str));
        ESP_LOGI(TAG, "wifi ip change :%s",wifi_ip);
    }
    if (mode == 1 && strlen(wifi_ip) > 0)
    {
        tcpip_adapter_ip_info_t wifi_ip_info;
        uint8_t ip_temp[4];

        ESP_LOGW(TAG,"set wifi Custom ip ->");
        // 第一步先关闭DHCP功能,默认是打开的
        tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA);
        // 第二步设置IP地址相关参数
        memset(ip_temp,0,4);
        ipstr_to_ip_address(wifi_ip,ip_temp);
        // IP4_ADDR(&wifi_ip_info.ip,192,168,137,200);
        IP4_ADDR(&wifi_ip_info.ip,ip_temp[0],ip_temp[1],ip_temp[2],ip_temp[3]);
        ESP_LOGI(TAG,"set device wifi ip address:%d.%d.%d.%d",ip_temp[0],ip_temp[1],ip_temp[2],ip_temp[3]);

        memset(ip_temp,0,4);
        ipstr_to_ip_address(wifi_gw,ip_temp);
        // IP4_ADDR(&wifi_ip_info.gw,192,168,0,1);
        IP4_ADDR(&wifi_ip_info.gw,ip_temp[0],ip_temp[1],ip_temp[2],ip_temp[3]);
        ESP_LOGI(TAG,"set device wifi gateway address:%d.%d.%d.%d",ip_temp[0],ip_temp[1],ip_temp[2],ip_temp[3]);

        memset(ip_temp,0,4);
        ipstr_to_ip_address(wifi_netmask,ip_temp);
        // IP4_ADDR(&wifi_ip_info.netmask,255,255,255,0);
        IP4_ADDR(&wifi_ip_info.netmask,ip_temp[0],ip_temp[1],ip_temp[2],ip_temp[3]);
        ESP_LOGI(TAG,"set device wifi netmask address:%d.%d.%d.%d",ip_temp[0],ip_temp[1],ip_temp[2],ip_temp[3]);

        tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA,&wifi_ip_info);
        //第三步打开dhcp
        tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_STA);
    }
    
    return retval;
}

/*
    ip_str = NULL,理解为查询网络状态
    retval = 1，在线
    retval = 0，掉线
*/
int wifi_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str)
{
    int retval = 0;
    if (ip_str == NULL || gw_str == NULL || netmask_str == NULL)
    {
    }
    else
    {
        memcpy(ip_str,s_WIFI_ip,4);
        memcpy(gw_str,s_WIFI_gateway,4);
        memcpy(netmask_str,s_WIFI_netmask,4);
    }
    retval = wifi_flag;
    return retval;
}

static void wifi_init(int set)
{
#if 1
    uint16_t ap_count = 0;
    if (set == 0)
    {
        ESP_LOGW(TAG, "WIFI exit <-- \n");
        esp_wifi_disconnect();
        return;
    }
    if (strlen(WIFI_name) == 0)
    {
        memset(WIFI_name,0,sizeof(WIFI_name));
        memset(WIFI_pass,0,sizeof(WIFI_pass));
    }
    
    ESP_LOGW(TAG, "WIFI Start --> \n");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    if (esp_netif_init_flag == 0)
    {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());   // 创建一个默认系统事件调度循环，之后可以注册回调函数来处理系统的一些事件
        esp_netif_init_flag = 1;
    }
    esp_netif_create_default_wifi_sta();    // 使用默认配置创建STA对象

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    // wifi logo
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_scan_start(NULL, true);
    esp_wifi_scan_stop();
    memset(wifi_ap_info, 0, sizeof(wifi_ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&wifi_ap_num, wifi_ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

    ESP_LOGI(TAG, "Total APs ap_count[%d],wifi_ap_num[%d]", ap_count,wifi_ap_num);
    for (int i = 0; (i < wifi_ap_num) && (i < ap_count); i++) {
        ESP_LOGI(TAG, "SSID \t\t%s,%d", wifi_ap_info[i].ssid,strlen((char *)wifi_ap_info[i].ssid));
        ESP_LOGI(TAG, "RSSI \t\t%d", wifi_ap_info[i].rssi);
        if (wifi_ap_info[i].rssi > (-86))
        {
            int temp_num = 0;
            print_auth_mode(wifi_ap_info[i].authmode);
            if (wifi_ap_info[i].authmode != WIFI_AUTH_WEP) {
                print_cipher_type(wifi_ap_info[i].pairwise_cipher, wifi_ap_info[i].group_cipher);
            }
            temp_num = strlen((char *)wifi_ap_info[i].ssid);
            if (temp_num == strlen(DEFAULT_WIFI_NAME))
            {
                if (memcmp((char *)wifi_ap_info[i].ssid,DEFAULT_WIFI_NAME,temp_num) == 0)
                {
                    memcpy(WIFI_name,DEFAULT_WIFI_NAME,strlen(DEFAULT_WIFI_NAME));
                    memcpy(WIFI_pass,DEFAULT_WIFI_PASS,strlen(DEFAULT_WIFI_PASS));
                    break;
                }
            }
            else if (temp_num == strlen(DEFAULT_WIFI_NAME2))
            {
                if (memcmp((char *)wifi_ap_info[i].ssid,DEFAULT_WIFI_NAME2,temp_num) == 0)
                {
                    memcpy(WIFI_name,DEFAULT_WIFI_NAME2,strlen(DEFAULT_WIFI_NAME2));
                    memcpy(WIFI_pass,DEFAULT_WIFI_PASS2,strlen(DEFAULT_WIFI_PASS2));
                    break;
                }
            }
        }
        ESP_LOGI(TAG, "Channel \t\t%d\n", wifi_ap_info[i].primary);
    }
    esp_wifi_stop();
    if (strlen(WIFI_name) == 0)
    {
        memcpy(WIFI_name,DEFAULT_WIFI_NAME,strlen(DEFAULT_WIFI_NAME));
        memcpy(WIFI_pass,DEFAULT_WIFI_PASS,strlen(DEFAULT_WIFI_PASS));
    }

    // 注册事件
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_event_handler,NULL,&instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,IP_EVENT_STA_GOT_IP,&wifi_event_handler,NULL,&instance_got_ip));
    // 静态IP
    if (strlen(wifi_ip) == 0)
    {
        memcpy(wifi_ip,"192.168.11.168",strlen("192.168.11.168"));
        memcpy(wifi_gw,"192.168.11.1",strlen("192.168.11.1"));
        memcpy(wifi_netmask,"255.255.255.0",strlen("255.255.255.0"));
        ESP_LOGI(TAG, "wifi ip no set,default wifi ip:%s",wifi_ip);
    }
    if (wifi_config_ip (0X0F,NULL,NULL,NULL) == 1)
    {
        wifi_config_ip (1,NULL,NULL,NULL);      // 静态ip有效后，执行更改ip
    }
    
    // 修改成自定义的SSID和密码
    wifi_config_t wifi_config = {
        .sta = {
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
	     .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    memcpy(wifi_config.sta.ssid,WIFI_name,strlen(WIFI_name));
    memcpy(wifi_config.sta.password,WIFI_pass,strlen(WIFI_pass));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WIFI Start, target [%s][%s] --> \n", wifi_config.sta.ssid,wifi_config.sta.password);
    ESP_LOGW(TAG, "wifi init sta finished");
#endif
}

static int eth_flag = 0;                // 1:rj45建立连接
// 静态的ip
static char eth_mode = 0;       // 0:dhcp   1:static
static char eth_ip[30];
static char eth_gw[30];
static char eth_netmask[30];

esp_netif_t *eth_netif;
// 这里是本次设备分配到的ip，很可能是动态的
static uint8_t s_RJ45_ip[4];
static uint8_t s_RJ45_netmask[4];
static uint8_t s_RJ45_gateway[4];

static char s_eth_DNS1_str[30];
static char s_eth_DNS2_str[30];

/*
    mode = 0,dhcp
    mode = 1,static 
    mode = other    查询mode
    ip_str  ip地址
    gw_str  默认网关
    netmask_str 掩码
*/
int eth_config_ip (char mode,char *ip_str,char *gw_str,char *netmask_str)
{
    int retval = 0;
    if (mode == 0)
    {
        eth_mode = 0;
        ESP_LOGW(TAG,"set eth dhcp ip");    // 路由器才会自动分配IP 
    }
    else if (mode == 1)
    {
        eth_mode = 1;
        ESP_LOGW(TAG,"set eth static ip");
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
        ESP_LOGI(TAG, "eth ip change :%s",eth_ip);
    }
    return retval;
}

int eth_config_DNS (char *DNS1,char *DNS2)
{
    int retval = 0;
    char temp_str[100];
    if (DNS1 == NULL || DNS2 == NULL)
    {
        return retval;
    }
    else
    {
        memset(s_eth_DNS1_str,0,sizeof(s_eth_DNS1_str));
        memset(s_eth_DNS2_str,0,sizeof(s_eth_DNS2_str));
        strcpy(s_eth_DNS1_str,DNS1);
        strcpy(s_eth_DNS2_str,DNS2);
    }
    return retval;
}

/*
    ip_str = NULL,理解为查询网络状态
    retval = 1，在线
    retval = 0，掉线
*/
int eth_get_local_ip_status (uint8_t *ip_str,uint8_t *gw_str,uint8_t *netmask_str)
{
    int retval = 0;
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
    return retval;
}

static void eth_Link_UP_handle (void *event_data)
{
    uint8_t mac_addr[6];
    char ip_temp[4];
    char ip_DNS[20];
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;
    port_8160_using = 0;
    
    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_ETH, "Cavendish eth"); // 实际在DHCP动态分配IP时候才生效，静态IP时候不生效
    if (eth_mode == 0)
    {
        ESP_LOGI(TAG, "Ethernet allocation ip : dhcp");
    }
    else
    {
        // 设置相关静态IP
        ESP_LOGI(TAG, "Ethernet allocation : static");
        if (strlen(eth_ip) == 0)
        {
            memcpy(eth_ip,"192.168.1.168",strlen("192.168.1.168"));
            memcpy(eth_gw,"192.168.1.1",strlen("192.168.1.1"));
            memcpy(eth_netmask,"255.255.255.0",strlen("255.255.255.0"));
        }
        if (strlen(s_eth_DNS1_str) == 0)
        {
            strcpy(s_eth_DNS1_str,"114.114.114.114");
        }
        memset(ip_DNS,0,sizeof(ip_DNS));
        strcpy(ip_DNS,s_eth_DNS1_str);
        tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_ETH);
        tcpip_adapter_ip_info_t eth;
        eth.ip.addr = ipaddr_addr(eth_ip);
        eth.netmask.addr = ipaddr_addr(eth_netmask);
        eth.gw.addr = ipaddr_addr(eth_gw);
        tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_ETH, &eth);

        // 设置DNS
        tcpip_adapter_dns_info_t dns_info = {0};
        memset(ip_temp, 0, 4);
        dns_info.ip.type = IPADDR_TYPE_V4;
        dns_info.ip.u_addr.ip4.addr = ipaddr_addr(ip_DNS);
        tcpip_adapter_set_dns_info(TCPIP_ADAPTER_IF_ETH, ESP_NETIF_DNS_FALLBACK, &dns_info);
    }
    memset(mac_addr,0,sizeof(mac_addr));
    esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
    ESP_LOGI(TAG, "Ethernet HW MAC Addr %02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    // rj45_tcp_client_task_init();
}

static void eth_event_handler(void *arg, esp_event_base_t event_base,int32_t event_id, void *event_data)
{
    /* we can get the ethernet driver handle from event data */
    if (event_base == ETH_EVENT)
    {
        switch (event_id)
        {
        case ETHERNET_EVENT_CONNECTED:
            ESP_LOGW(TAG, "Ethernet Link up");
            eth_Link_UP_handle (event_data);
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Ethernet Link Down \n");
            break;
        case ETHERNET_EVENT_START:
            ESP_LOGI(TAG, "Ethernet Started");
            break;
        case ETHERNET_EVENT_STOP:
            ESP_LOGI(TAG, "Ethernet Stopped \n");
            break;
        default:
            break;
        }
        eth_flag = 0;
        memset(s_RJ45_ip,0,sizeof(s_RJ45_ip));
        memset(s_RJ45_netmask,0,sizeof(s_RJ45_netmask));
        memset(s_RJ45_gateway,0,sizeof(s_RJ45_gateway));
    }
    
    if (event_base == IP_EVENT)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        const esp_netif_ip_info_t *ip_info = &event->ip_info;
        switch (event_id)
        {
        case IP_EVENT_ETH_GOT_IP:
            s_RJ45_ip[0] = esp_ip4_addr1_16(&ip_info->ip);
            s_RJ45_ip[1] = esp_ip4_addr2_16(&ip_info->ip);
            s_RJ45_ip[2] = esp_ip4_addr3_16(&ip_info->ip);
            s_RJ45_ip[3] = esp_ip4_addr4_16(&ip_info->ip);

            s_RJ45_netmask[0] = esp_ip4_addr1_16(&ip_info->netmask);
            s_RJ45_netmask[1] = esp_ip4_addr2_16(&ip_info->netmask);
            s_RJ45_netmask[2] = esp_ip4_addr3_16(&ip_info->netmask);
            s_RJ45_netmask[3] = esp_ip4_addr4_16(&ip_info->netmask);

            s_RJ45_gateway[0] = esp_ip4_addr1_16(&ip_info->gw);
            s_RJ45_gateway[1] = esp_ip4_addr2_16(&ip_info->gw);
            s_RJ45_gateway[2] = esp_ip4_addr3_16(&ip_info->gw);
            s_RJ45_gateway[3] = esp_ip4_addr4_16(&ip_info->gw);
            
            ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
            ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
            ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
            ESP_LOGW(TAG, "Ethernet Got IP Address \n");
            eth_flag = 1;
            break;
        default:
            break;
        }
    }
}

static void rtl8201_init(int set)
{
#if 1
    uint8_t mac_addr[6];
    int temp_num = 0;
    if (set == 0)
    {
        ESP_LOGW(TAG, "RJ45 exit <-- \n");
        return;
    }
    
    ESP_LOGW(TAG, "RJ45 Start --> \n");
    if (SYS_MAC_addr[0] + SYS_MAC_addr[1] + SYS_MAC_addr[3] + SYS_MAC_addr[4])
    {
        memcpy(mac_addr,SYS_MAC_addr,sizeof(mac_addr));
    }
    else
    {
        ESP_LOGW(TAG, "RJ45 no mac addr");
        // 随机mac模式
    }
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    if (esp_netif_init_flag == 0)
    {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_init_flag = 1;
    }

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    eth_netif = esp_netif_new(&cfg);
    // Set default handlers to process TCP/IP stuffs
    // ESP_ERROR_CHECK(esp_eth_set_default_handlers(eth_netif));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &eth_event_handler, NULL));

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG(); // ETH_MAC_DEFAULT_CONFIG 这个函数里面定义了MDC和MDIO引脚，这些RMII接口引脚是固定引脚，所以写在底层函数中，不能改变
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG(); // 这个ETH_PHY_DEFAULT_CONFIG函数中定义了LAN8720复位引脚和phy_addr 获取方式

    #if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET
    mac_config.smi_mdc_gpio_num = RTL8201_ETH_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = RTL8201_ETH_MDIO_GPIO;
    phy_config.reset_gpio_num = RTL8201_ETH_RST_GPIO;
    phy_config.phy_addr = RTL8201_ETH_PHY_ADDR;
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);

    #if CONFIG_EXAMPLE_ETH_PHY_IP101
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
    #elif CONFIG_EXAMPLE_ETH_PHY_RTL8201
    esp_eth_phy_t *phy = esp_eth_phy_new_rtl8201(&phy_config);
    #elif CONFIG_EXAMPLE_ETH_PHY_LAN8720
    esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);
    #elif CONFIG_EXAMPLE_ETH_PHY_DP83848
    esp_eth_phy_t *phy = esp_eth_phy_new_dp83848(&phy_config);
    #elif CONFIG_EXAMPLE_ETH_PHY_KSZ8041
    esp_eth_phy_t *phy = esp_eth_phy_new_ksz8041(&phy_config);
    #endif

    #endif
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
    esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, mac_addr);
    ESP_LOGW(TAG, "IP mac addr : %02X:%02X:%02X:%02X:%02X:%02X", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    /* attach Ethernet driver to TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));
    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    ESP_LOGW(TAG, "RJ45 init finished");
#endif
}

void Network_manage_set_mac (uint8_t *mac)
{
    if (mac != NULL)
    {
        memcpy(SYS_MAC_addr,mac,6);
        ESP_LOGI(TAG, "set mac: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
}

void Network_manage_get_mac (uint8_t *mac)
{
    if (mac != NULL)
    {
        memcpy(mac,SYS_MAC_addr,6);
    }
}

/*
    retval = 0,无网络
    retval = 2,wifi连接
    retval = 4,rj45连接
*/
int Network_manage_get_status (void)
{
    int retval = 0,temp = 0;
    if (wifi_get_local_ip_status(NULL,NULL,NULL))
    {
        retval |= 0x01 << 1;
    }
    else
    {
        temp = 0x01 << 1;
        retval &= ~temp;
    }
    if (eth_get_local_ip_status(NULL,NULL,NULL))
    {
        retval |= 0x01 << 2;
    }
    else
    {
        temp = 0x01 << 2;
        retval &= ~temp;
    }
    // if (Net_4G_get_local_ip_status (NULL,NULL,NULL))
    // {
    //     retval |= 0x01 << 3;
    // }
    // else
    // {
    //     temp = 0x01 << 3;
    //     retval &= ~temp;
    // }
    // debug_log(LOG_View,TAG,"get status [%d]",retval);
    return retval;
}

int Network_manage_Init (int mode,int set)
{
    int retval = 0;

    switch (mode)
    {
    case 1:
        wifi_init(set);
        break;
    case 2:
        rtl8201_init(set);
        break;
    case 0xFE:
        esp_wifi_stop();
        wifi_flag = 0;
        break;
    case 0xFF:
        eth_flag = 0;
        break;
    default:
        esp_wifi_stop();
        eth_flag = 0;
        wifi_flag = 0;
        break;
    }
    return retval;
}
