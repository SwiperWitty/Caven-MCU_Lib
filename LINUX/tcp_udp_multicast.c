/*
    前提是eth和wifi任意一个启动连接才可以使用
*/

#include "tcp_udp_multicast.h"
#include "debug_log.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

// 

static const char *TAG = "TCP udp multicast";

static int tcp_udp_enable = 0;
static int tcp_udp_Sd_enable = 0;
static int tcp_udp_sockfd = -1;
struct sockaddr_in broadcast_addr;

static char sock_ip_Sd_str[50] = {0};
static char sock_port_Sd_str[10] = {0};
static char sock_ip_Rv_str[50] = {0};
static char sock_port_Rv_str[10] = {0};

/*
    server 只能修改端口，如果需要修改ip请修改[eth_config_ip]/[wifi_config_ip]
    enable 0 会关闭当前sock，没有则不生效
*/
int tcp_udp_multicast_Sd_config (char *ip_str,char *port_str,int enable)
{
    int retval = 0;
    if (enable == 0)
    {
        tcp_udp_Sd_enable = enable;
    }
    else
    {
        if (port_str == NULL)
        {
            // ESP_LOGE(TAG, "where are you IP ?");
            return retval;
        }
        else
        {
            tcp_udp_Sd_enable = enable;
            memset(sock_ip_Sd_str,0,sizeof(sock_ip_Sd_str));
            strcpy(sock_ip_Sd_str, ip_str);
            memset(sock_port_Sd_str,0,sizeof(sock_port_Sd_str));
            strcpy(sock_port_Sd_str, port_str);
            debug_log(LOG_Info,TAG,"config link ip[%s:%s]",ip_str,port_str);

        }
    }
    return retval;
}

int tcp_udp_multicast_Rv_config (char *ip_str,char *port_str,int enable)
{
    int retval = 0;
    if (enable == 0)
    {
        tcp_udp_enable = enable;

        if (tcp_udp_sockfd >= 0)
        {
            shutdown(tcp_udp_sockfd, 0);
            close(tcp_udp_sockfd);
            tcp_udp_sockfd = -1;
            debug_log(LOG_Warn,TAG,"close sockfd");
        }
    }
    else
    {
        if (port_str == NULL)
        {
            if (tcp_udp_sockfd >= 0)
            {
                retval = 1;
            }
            // ESP_LOGE(TAG, "where are you IP ?");
            return retval;
        }
        else
        {
            tcp_udp_enable = enable;
            memset(sock_ip_Rv_str,0,sizeof(sock_ip_Rv_str));
            strcpy(sock_ip_Rv_str, ip_str);
            memset(sock_port_Rv_str,0,sizeof(sock_port_Rv_str));
            strcpy(sock_port_Rv_str, port_str);
            debug_log(LOG_Info,TAG,"config link ip[%s:%s]",ip_str,port_str);
            if (tcp_udp_sockfd > 0)
            {
                shutdown(tcp_udp_sockfd, 0);
                close(tcp_udp_sockfd);
            }
        }
    }
    if (tcp_udp_sockfd >= 0)
    {
        retval = 1;
    }
    return retval;
}

/*
    使用前确保网络通畅
*/
int tcp_udp_multicast_send_data(uint8_t *data, int size)
{
    int retval = 0;
    if (data == NULL || size == 0 || tcp_udp_Sd_enable == 0)
    {
        retval = 1;
        return retval;
    }
    else
    {
        int sockfd,ip_port;
        struct sockaddr_in multicast_addr;
        if (strlen(sock_ip_Sd_str) <= 0)
        {
            strcpy(sock_ip_Sd_str,"224.0.0.1");
        }
        if (strlen(sock_port_Sd_str) <= 0)
        {
            strcpy(sock_port_Sd_str,"8161");
        }
        ip_port = atoi(sock_port_Sd_str);
        debug_log(LOG_Info,TAG,"ip %s:%d",sock_ip_Sd_str,ip_port);
        // 创建 UDP 套接字
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("Socket creation failed");
        }

        // 设置组播地址和端口
        memset(&multicast_addr, 0, sizeof(multicast_addr));
        multicast_addr.sin_family = AF_INET;
        multicast_addr.sin_port = htons(ip_port);
        multicast_addr.sin_addr.s_addr = inet_addr(sock_ip_Sd_str);

        // 设置 TTL（可选）
        int ttl = 1; // TTL=1 表示仅限于本地网络
        if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
            perror("Setsockopt (IP_MULTICAST_TTL) failed");
            close(sockfd);
        }

        // 发送组播消息
        if (sendto(sockfd, data, size, 0, (struct sockaddr *)&multicast_addr, sizeof(multicast_addr)) < 0) {
            perror("Sendto failed");
            close(sockfd);
        }
        // 关闭套接字
        if (sockfd >= 0)
        {
            close(sockfd);
        }
    }
    return retval;
}

/*
    接收回调函数执行指针
*/
static D_pFun tcp_udp_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void tcp_udp_multicast_receive_State_Machine_Bind (D_pFun Callback_pFun)
{
    tcp_udp_Callback_Fun = Callback_pFun;
}

/*
    网络的应用层任务必须先确保底层网络是启动的，否则不应该启动这个任务
*/
void *tcp_udp_multicast_task(void *empty)
{
    char cmd_str[128];
    int ip_port = 0;
    int temp_num = 0;
    int tcp_rst = 0;

    uint8_t data[10240];
    GT_tcp_server_start:
    debug_log(LOG_Info,TAG,"tcp_udp_link_task");
    while (tcp_udp_enable == 0)
    {
        sleep(1);
    }
    if (strlen(sock_ip_Rv_str) <= 0)
    {
        strcpy(sock_ip_Rv_str,"224.0.0.1");
    }
    if (strlen(sock_port_Rv_str) <= 0)
    {
        strcpy(sock_port_Rv_str,"8162");
    }
    ip_port = atoi(sock_port_Rv_str);
    struct sockaddr_in server_addr;

    // 创建 UDP 套接字
    if ((tcp_udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
    }

    // 绑定到本地端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(ip_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(tcp_udp_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(tcp_udp_sockfd);
    }

    // 加入组播组
    struct ip_mreq multicast_group;
    multicast_group.imr_multiaddr.s_addr = inet_addr(sock_ip_Rv_str);
    multicast_group.imr_interface.s_addr = INADDR_ANY;
    if (setsockopt(tcp_udp_sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_group, sizeof(multicast_group)) < 0) {
        perror("Setsockopt (IP_ADD_MEMBERSHIP) failed");
        close(tcp_udp_sockfd);
    }
    debug_log(LOG_Info,TAG,"ip %s:%d",sock_ip_Rv_str,ip_port);
    
    while(1)
    {
        if (tcp_udp_sockfd >= 0)
        {
            int temp_num = recvfrom(tcp_udp_sockfd, data, sizeof(data) - 1, 0,NULL, 0);
            if (temp_num > 0) {
                if (tcp_udp_Callback_Fun != NULL)
                {
                    for (int i = 0; i < temp_num; i++)
                    {
                        tcp_udp_Callback_Fun(&data[i]);
                    }
                }
            }
            else
            {
                perror("udp Recvfrom");     // net loss
                break;
            }
        }
        else
        {
            break;
        }
    }
    if (tcp_udp_sockfd >= 0)
    {
        close(tcp_udp_sockfd);
    }
    sleep(2);
    goto GT_tcp_server_start;
}
