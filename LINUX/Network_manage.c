#include "Network_manage.h"
#include "debug_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static const char *TAG = "Network_manage";

// //

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

// net_4g //

static char net_4G_ip[30];
static char net_4G_gw[30];
static char net_4G_netmask[30];
static char net_4G_dns1[30];
static char net_4G_dns2[30];

static int net_4G_flag = 0;
static int net_4G_enable = 0;

void Net_4G_manage_Init(int set)
{
    char cmd_str[100];
    memset(cmd_str,0,sizeof(cmd_str));
    
    if (set)
    {
        if (net_4G_enable == 0)
        {
            system("echo ON > /dev/4g_pwr");
            debug_log(LOG_Info,TAG,"4g up");
            net_4G_enable = 1;
        }
    }
    else
    {
        if (net_4G_enable == 1)
        {
            system("echo OFF > /dev/4g_pwr");
            debug_log(LOG_Info,TAG,"4g down");
            net_4G_flag = 0;
            net_4G_enable = 0;
        }
    }
}

/*
    ip_str = NULL,理解为查询网络状态
    retval = 1，在线
    retval = 0，掉线
*/
int Net_4G_get_local_ip_status (uint8_t *ip,uint8_t *gw,uint8_t *netmask)
{
    int retval = 0;
    char temp_str[500];
    char temp_array[200];
    char *pointer = NULL;
    char *offset = NULL;
    if (net_4G_enable == 0)
    {
        net_4G_flag = 0;
        return retval;
    }
    
    memset(temp_str,0,sizeof(temp_str));
    my_system("ifconfig usb0",temp_str,sizeof(temp_str));
    if (ip == NULL || gw == NULL || netmask == NULL)
    {
        offset = strstr(temp_str,"Link encap:Ethernet");
        if (offset != NULL)
        {
            net_4G_flag = 1;
        }
        else
        {
            net_4G_flag = 0;
        }
    }
    else
    {
        offset = strstr(temp_str,"inet addr:");
        if (offset != NULL)
        {   
            net_4G_flag = 1;
            pointer = strstr(offset,"Bcast:");
            if(pointer != NULL)
            {
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,offset,pointer - offset);
                Caven_Str_To_ip (temp_array+strlen("inet addr:"),ip,4);
            }
        }
        offset = strstr(temp_str,"Bcast:");
        if (offset != NULL)
        {
            pointer = strstr(offset,"Mask:");
            if(pointer != NULL)
            {
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,offset,pointer - offset);
                Caven_Str_To_ip (temp_array+strlen("Bcast:"),gw,4);
            }
        }
        offset = strstr(temp_str,"Mask:");
        if (offset != NULL)
        {
            pointer = strstr(offset,"\n");
            if(pointer != NULL)
            {
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,offset,pointer - offset);
                Caven_Str_To_ip (temp_array+strlen("Mask:"),netmask,4);
            }
        }
    }
    retval = net_4G_flag;
    return retval;
}

// wifi //

static int wifi_flag = 0;               // 1:wifi建立连接
static int wifi_enable = 0;

static char wifi_mode = 0;      // 0:dhcp   1:static

static char wifi_ip[30];
static char wifi_gw[30];
static char wifi_netmask[30];

static char WIFI_name[30] = {0};
static char WIFI_pass[30] = {0};
// 这里是本次设备分配到的ip，很可能是动态的
static uint8_t s_WIFI_ip[4];
static uint8_t s_WIFI_netmask[4];
static uint8_t s_WIFI_gateway[4];

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
        // ESP_LOGW(TAG,"set wifi dhcp ip");    // 路由器才会自动分配IP 
    }
    else if (mode == 1)
    {
        wifi_mode = 1;
        // ESP_LOGW(TAG,"set wifi static ip");
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
        // ESP_LOGI(TAG, "wifi ip change :%s",wifi_ip);
    }
    return retval;
}

int wifi_Account_config (char *name,char *pass)
{
    int retval = 0;
    if (name == NULL || pass == NULL)
    {
        return retval;
    }
    else
    {
        memset(WIFI_name,0,sizeof(WIFI_name));
        memset(WIFI_pass,0,sizeof(WIFI_pass));
        strcpy(WIFI_name,name);
        strcpy(WIFI_pass,pass);
        FILE *f = NULL;
        f = fopen("/etc/wifi/wpa.conf", "w");
        if (f == NULL) {
            debug_log(LOG_Warn,TAG, "Failed to open file for writing");
        }
        else
        {
            fprintf(f, "ctrl_interface=/var/run/wpa_supplicant\r\n\r\nnetwork={\r\nssid=\"%s\"\r\npsk=\"%s\"\r\nkey_mgmt=WPA-PSK\r\npriority=2 \r\n}",
            WIFI_name,WIFI_pass);  // 写入内容
            debug_log(LOG_Warn,TAG, "reset wifi Account[%s][%s]",WIFI_name,WIFI_pass);
            fclose(f);
        }
    }
    return retval;
}

void wifi_scan_ssid (char *str,int str_size)
{
    if (wifi_enable == 0)
    {
        return ;
    }
    my_system("wpa_cli -i wlan0 scan && sleep 1 && wpa_cli -i wlan0 scan_results",str,str_size);
}

int wifi_get_link_name (char *str)
{
    int retval = 0;
    char temp_str[300];
    char link_wifi_name[50];
    char *temp_pointer = NULL;
    if (str == NULL || wifi_enable == 0)
    {
        return retval;
    }
    
    memset(temp_str,0,sizeof(temp_str));
    memset(link_wifi_name,0,sizeof(link_wifi_name));
    my_system("iw dev wlan0 link",temp_str,sizeof(temp_str));
    temp_pointer = strstr(temp_str,"SSID: ");
    if (temp_pointer != NULL)
    {
        temp_pointer += strlen("SSID: ");
        for (int i = 0; i < strlen(temp_pointer); i++)
        {
            if (i >= sizeof(link_wifi_name))
            {
                break;
            }
            if (temp_pointer[i] == '\n')
            {
                break;
            }
            else
            {
                link_wifi_name[i] = temp_pointer[i];
            }
            retval = 1;
        }
        strcpy(str,link_wifi_name);
    }
    return retval;
}

/*
    ip = NULL,理解为查询网络状态
    retval = 1，在线
    retval = 0，掉线
*/
int wifi_get_local_ip_status (uint8_t *ip,uint8_t *gw,uint8_t *netmask)
{
    int retval = 0;
    char temp_str[500];
    memset(temp_str,0,sizeof(temp_str));
    if (wifi_enable == 0)
    {
        wifi_flag = 0;
        return retval;
    }
    
    if (ip == NULL || gw == NULL || netmask == NULL)
    {
        if (wifi_get_link_name (temp_str) > 0)
        {
            wifi_flag = 1;
        }
        else
        {
            wifi_flag = 0;
        }
    }
    else
    {
        char temp_array[200];
        char *pointer = NULL;
        char *offset = NULL;

        my_system("ifconfig wlan0",temp_str,sizeof(temp_str));
        offset = strstr(temp_str,"inet addr:");
        if (offset != NULL)
        {
            pointer = strstr(offset,"Bcast:");
            if(pointer != NULL)
            {
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,offset,pointer - offset);
                debug_log(LOG_Info,TAG,"wifi %s",temp_array);
                Caven_Str_To_ip (temp_array+strlen("inet addr:"),ip,4);
            }
        }
        offset = strstr(temp_str,"Bcast:");
        if (offset != NULL)
        {
            pointer = strstr(offset,"Mask:");
            if(pointer != NULL)
            {
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,offset,pointer - offset);
                debug_log(LOG_Info,TAG,"wifi %s",temp_array);
                Caven_Str_To_ip (temp_array+strlen("Bcast:"),gw,4);
            }
        }
        offset = strstr(temp_str,"Mask:");
        if (offset != NULL)
        {
            pointer = strstr(offset,"\n");
            if(pointer != NULL)
            {
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,offset,pointer - offset);
                debug_log(LOG_Info,TAG,"wifi %s",temp_array);
                Caven_Str_To_ip (temp_array+strlen("Mask:"),netmask,4);
            }
        }
    }
    retval = wifi_flag;
    return retval;
}

static void wifi_init(int set)
{
    char cmd_str[100];
    memset(cmd_str,0,sizeof(cmd_str));
    debug_log(LOG_Warn,TAG, "wifi init[%d] \n",set);
    if (set)
    {
        if (wifi_enable == 0)
        {
            system("ifconfig wlan0 up");
            // system("start-stop-daemon --start --quiet --background --exec /bin/sh -- /etc/wifi/wifi_sta_start");
            my_systemd("/etc/wifi/wifi_sta_start");
            if (wifi_mode == 0)
            {
                // system("start-stop-daemon --start --quiet --background --pidfile /tmp/wlan0_udhcpc.pid --make-pidfile --exec udhcpc -- -i wlan0");
                // my_systemd("sleep 1 && udhcpc -i wlan0 &");
            }
            else
            {
                strcat(cmd_str,"ifconfig wlan0 ");
                if (strlen(wifi_ip) == 0)
                {
                    strcat(wifi_ip,"192.168.11.168");
                    debug_log(LOG_Info,TAG,"wifi_ip default");
                }
                strcat(cmd_str,wifi_ip);
                strcat(cmd_str," netmask ");
                if (strlen(wifi_netmask) == 0)
                {
                    strcat(wifi_netmask,"255.255.255.0");
                    debug_log(LOG_Info,TAG,"wifi_netmask default");
                }
                strcat(cmd_str,wifi_netmask);
                debug_log(LOG_Info,TAG,"cmd[%s]",cmd_str);
                system(cmd_str);    //
                memset(cmd_str,0,sizeof(cmd_str));
                strcat(cmd_str,"route add default gw ");
                if (strlen(wifi_gw) == 0)
                {
                    strcat(wifi_gw,"192.168.11.1");
                    debug_log(LOG_Info,TAG,"wifi_gw default");
                }
                strcat(cmd_str,wifi_gw);
                debug_log(LOG_Info,TAG,"cmd[%s]",cmd_str);
                system(cmd_str);    //
            }
            wifi_enable  = 1;
        }
    }
    else
    {
        if (wifi_enable)
        {
            my_systemd("ifconfig wlan0 down");
            wifi_enable = 0;
        }
    }
}

// rj45 //
// 这里是本次设备分配到的ip，很可能是动态的
static uint8_t s_RJ45_ip[4];
static uint8_t s_RJ45_netmask[4];
static uint8_t s_RJ45_gateway[4];

static int eth_flag = 0;                // 1:rj45建立连接
// 静态的ip
static uint8_t SYS_MAC_addr[6] = {0};
static char eth_mode = 0;       // 0:dhcp   1:static

static char eth_ip[30];
static char eth_gw[30];
static char eth_netmask[30];

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
        FILE *f = NULL;
        memset(temp_str,0,sizeof(temp_str));
        f = fopen("/etc/resolv.conf", "a+");
        if (f == NULL) {
            debug_log(LOG_Warn,TAG, "Failed to open file for writing");
        }
        else
        {
            sprintf(temp_str,"nameserver %s\nnameserver %s\n",DNS1,DNS2);
            fputs(temp_str, f);
            fclose(f);
        }
    }
    return retval;
}

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
        // ESP_LOGW(TAG,"set eth dhcp ip");    // 路由器才会自动分配IP 
    }
    else if (mode == 1)
    {
        eth_mode = 1;
        // ESP_LOGW(TAG,"set eth static ip");
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
        // ESP_LOGI(TAG, "eth ip change :%s",eth_ip);
    }
    return retval;
}

static void rj45_init(int set)
{
    char cmd_str[100];
    memset(cmd_str,0,sizeof(cmd_str));
    
    if (set)
    {
        system("ifconfig eth0 up");
        if (eth_mode == 0)
        {
            system("start-stop-daemon --start --quiet --background --pidfile /tmp/eth0_udhcpc.pid --make-pidfile --exec udhcpc -- -i eth0");
        }
        else
        {
            strcat(cmd_str,"ifconfig eth0 ");
            if (strlen(eth_ip) == 0)
            {
                strcat(eth_ip,"192.168.1.168");
                debug_log(LOG_Info,TAG,"eth_ip default");
            }
            strcat(cmd_str,eth_ip);
            strcat(cmd_str," netmask ");
            if (strlen(eth_netmask) == 0)
            {
                strcat(eth_netmask,"255.255.255.0");
                debug_log(LOG_Info,TAG,"eth_netmask default");
            }
            strcat(cmd_str,eth_netmask);
            debug_log(LOG_Info,TAG,"cmd[%s]",cmd_str);
            system(cmd_str);    //
            memset(cmd_str,0,sizeof(cmd_str));
            strcat(cmd_str,"route add default gw ");
            if (strlen(eth_gw) == 0)
            {
                strcat(eth_gw,"192.168.1.1");
                debug_log(LOG_Info,TAG,"eth_gw default");
            }
            strcat(cmd_str,eth_gw);
            debug_log(LOG_Info,TAG,"cmd[%s]",cmd_str);
            system(cmd_str);    //
            system("ip route delet default via 192.168.1.1 dev eth0");      // !!!!
        }
        // get ip [s_RJ45_ip][s_RJ45_gateway][s_RJ45_netmask]
    }
    else
    {
        system("ifconfig eth0 down");
        debug_log(LOG_Info,TAG,"eth0 down");
        eth_flag = 0;
    }
}

void Network_manage_set_mac (uint8_t *mac)
{
    if (mac != NULL)
    {
        memcpy(SYS_MAC_addr,mac,6);
        // ESP_LOGI(TAG, "set mac: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
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
    ip = NULL,理解为查询网络状态 (linux 无意义)
    retval = 1，在线
    retval = 0，掉线
*/
int eth_get_local_ip_status (uint8_t *ip,uint8_t *gw,uint8_t *netmask)
{
    int retval = 0;
    char temp_str[500];
    memset(temp_str,0,sizeof(temp_str));
    if (ip == NULL || gw == NULL || netmask == NULL)
    {
        my_system("cat /sys/class/net/eth0/carrier",temp_str,sizeof(temp_str));
        eth_flag = atoi(temp_str);
        if (eth_flag)
        {
            eth_flag = 1;
        }
    }
    else
    {
        char temp_array[200];
        char *pointer = NULL;
        char *offset = NULL;

        my_system("ifconfig eth0",temp_str,sizeof(temp_str));
        offset = strstr(temp_str,"inet addr:");
        if (offset != NULL)
        {
            pointer = strstr(offset,"Bcast:");
            if(pointer != NULL)
            {
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,offset,pointer - offset);
                debug_log(LOG_Info,TAG,"eth %s",temp_array);
                Caven_Str_To_ip (temp_array+strlen("inet addr:"),ip,4);
            }
        }
        offset = strstr(temp_str,"Bcast:");
        if (offset != NULL)
        {
            pointer = strstr(offset,"Mask:");
            if(pointer != NULL)
            {
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,offset,pointer - offset);
                debug_log(LOG_Info,TAG,"eth %s",temp_array);
                Caven_Str_To_ip (temp_array+strlen("Bcast:"),gw,4);
            }
        }
        offset = strstr(temp_str,"Mask:");
        if (offset != NULL)
        {
            pointer = strstr(offset,"\n");
            if(pointer != NULL)
            {
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,offset,pointer - offset);
                debug_log(LOG_Info,TAG,"eth %s",temp_array);
                Caven_Str_To_ip (temp_array+strlen("Mask:"),netmask,4);
            }
        }
    }

    retval = eth_flag;
    return retval;
}

// //
/*
    retval = 0,无网络
    retval = 1,wifi连接
    retval = 2,rj45连接
    retval = 3,wifi+rj45
    retval = 4,4g
    retval = 5,wifi+4g
    retval = 6,rj45+4g
    retval = 7,wifi+rj45+4g
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
    if (Net_4G_get_local_ip_status (NULL,NULL,NULL))
    {
        retval |= 0x01 << 3;
    }
    else
    {
        temp = 0x01 << 3;
        retval &= ~temp;
    }
    debug_log(LOG_View,TAG,"get status [%d]",retval);
    return retval;
}

// 1 wifi,2 rj45
int Network_manage_Init (int network,int set)
{
    int retval = 0;

    switch (network)
    {
    case 1:
        wifi_init(set);
        break;
    case 2:
        rj45_init(set);
        break;
    case 3:
        Net_4G_manage_Init(set);
        break;
    default:
        // esp_wifi_stop();
        eth_flag = 0;
        wifi_flag = 0;
        break;
    }
    return retval;
}

int eth_enable_last = 0,wifi_enable_last = 0,net_4g_enable_last = 0;
int eth_enable_Intask = 0,wifi_enable_Intask = 0,net_4g_enable_Intask = 0;
int net_link_status = 0;
int Network_manage_wifi_enable_Intask (int set)
{
    wifi_enable_Intask = set;
}

int Network_manage_net_4G_enable_Intask (int set)
{
    net_4g_enable_Intask = set;
}

int Network_manage_get_link_Intask (void)
{
    return net_link_status;
}

int first_net = 0;
void *Network_manage_task(void *empty)
{
    int temp_run = 0;
    debug_log(LOG_Info,TAG,"Network_manage_task doing");
    while (1)
    {
        if (wifi_enable_last != wifi_enable_Intask)
        {
            wifi_enable_last = wifi_enable_Intask;
            wifi_init(wifi_enable_Intask);
        }
        if (net_4g_enable_last != net_4g_enable_Intask)
        {
            net_4g_enable_last = net_4g_enable_Intask;
            Net_4G_manage_Init (net_4g_enable_Intask);
        }
        
        if (net_4G_enable)
        {
            if (net_4G_flag == 0)
            {
                char temp_str[200];
                debug_log(LOG_Info,TAG,"net 4g lte_start");
                sleep(4);
                
                if (Net_4G_get_local_ip_status (NULL,NULL,NULL) == 0 && first_net == 0)
                {
                    first_net = 1;
                    do
                    {
                        sleep(4);
                        memset(temp_str,0,sizeof(temp_str));
                        my_system("ls /dev/ttyUSB*",temp_str,sizeof(temp_str));
                    } while (strstr(temp_str,"ttyUSB2") == NULL);
                    system("/etc/ppp/quectel/quectel-pppd.sh  /dev/ttyUSB2 &");
                    sleep(8);
                    system("/etc/ppp/lte_start");
                    sleep(4);
                    my_system("ip route delet default via 192.168.1.1 dev eth0",temp_str,sizeof(temp_str));
                }
                sleep(5);
                if (Net_4G_get_local_ip_status (NULL,NULL,NULL) > 0)
                {
                    debug_log(LOG_Info,TAG,"4G succ init");
                }
            }
        }
        else
        {

        }
        net_link_status = Network_manage_get_status();
        sleep(2);
    }
}
