/*
    前提是eth和wifi任意一个启动连接才可以使用
*/

#include "tcp_udp_link.h"
#include "debug_log.h"

#include <string.h>

// 

static const char *TAG = "TCP udp";

static int tcp_udp_enable = 0;
static int tcp_udp_sockfd = -1;
struct sockaddr_in broadcast_addr;

static char sock_ip_str[50] = {0};
static char sock_port_str[10] = {0};

/*
    server 只能修改端口，如果需要修改ip请修改[eth_config_ip]/[wifi_config_ip]
    enable 0 会关闭当前sock，没有则不生效
*/
int tcp_udp_link_config (char *ip_str,char *port_str,int enable)
{
    int retval = 0;
    if (enable == 0)
    {
        tcp_udp_enable = enable;

        if (tcp_udp_sockfd >= 0)
        {
            shutdown(tcp_udp_sockfd, 0);
            close(tcp_udp_sockfd);
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
            memset(sock_ip_str,0,sizeof(sock_ip_str));
            strcpy(sock_ip_str, ip_str);
            memset(sock_port_str,0,sizeof(sock_port_str));
            strcpy(sock_port_str, port_str);
            debug_log(LOG_Info,TAG,"config link ip[%s:%s]",ip_str,sock_port_str);
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
int tcp_udp_send_data(uint8_t *data, int size)
{
    int retval = 0;
    int len = size;
    if (data == NULL || size == 0)
    {
        retval = 1;
        return retval;
    }
    
    if (tcp_udp_sockfd >= 0 && tcp_udp_enable)
    {
        retval = sendto(tcp_udp_sockfd,data,size,0,(struct sockaddr*)&broadcast_addr,sizeof(struct sockaddr_in));
        if (retval < 0)
        {
            perror("Sendto failed");
            close(tcp_udp_sockfd);
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
void tcp_udp_receive_State_Machine_Bind (D_pFun Callback_pFun)
{
    tcp_udp_Callback_Fun = Callback_pFun;
}

/*
    网络的应用层任务必须先确保底层网络是启动的，否则不应该启动这个任务
*/
void *tcp_udp_link_task(void *empty)
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
    if (strlen(sock_ip_str) <= 0)
    {
        strcpy(sock_ip_str,"255.255.255.255");
    }
    if (strlen(sock_port_str) <= 0)
    {
        strcpy(sock_port_str,"9960");
    }
    ip_port = atoi(sock_port_str);
    
    //1、创建一个套接字文件描述符
    tcp_udp_sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(tcp_udp_sockfd < 0)
    {
        perror("socket fail");
    }
    // 设置套接字为广播模式
    int broadcast_enable = 1;
    if (setsockopt(tcp_udp_sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
        perror("Setsockopt (SO_BROADCAST) failed");
        close(tcp_udp_sockfd);
    }

    // 设置广播地址和端口
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_port = htons(ip_port);
    broadcast_addr.sin_addr.s_addr = inet_addr(sock_ip_str);   // 255 or INADDR_ANY
    struct sockaddr_in client_addr;
    memset(&client_addr,0,sizeof(client_addr));
    socklen_t client_len = sizeof(client_addr);

    debug_log(LOG_Info,TAG,"ip %s:%d",sock_ip_str,ip_port);
    tcp_udp_send_data("udp update\n", strlen("udp update\n"));
    while(1)
    {
        if (tcp_udp_sockfd >= 0)
        {
            int temp_num = recvfrom(tcp_udp_sockfd, data, sizeof(data) - 1, 0, (struct sockaddr *)&client_addr, &client_len);
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
                perror("udp connect loss");     // net loss
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
