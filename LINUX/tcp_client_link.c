/*
    前提是eth和wifi任意一个启动连接才可以使用
*/

#include "tcp_client_link.h"
#include "debug_log.h"

#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>

// 

#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR "192.168.1.128"
#elif defined(CONFIG_EXAMPLE_IPV6)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#else
#define HOST_IP_ADDR ""
#endif

static const char *TAG = "TCP client";

static int tcp_client_enable = 0;
static char sock_ip_str[50] = {0};
static char sock_port_str[10] = {0};
static char sock_flag = 0;      // 1是wifi的sock

static int client_sockfd = -1;

/*
    enable = 0,关闭client_link,并清除ip_str
    enable = 1,打开client_link,并重置ip_str,close sock
*/
int tcp_client_link_config (char *ip_str,char *port_str,int enable)
{
    int retval = 0;
    if (enable == 0)
    {
        tcp_client_enable = enable;
        if (client_sockfd >= 0)
        {
            shutdown(client_sockfd, 0);
            close(client_sockfd);
            client_sockfd = -1;
            debug_log(LOG_Warn,TAG,"close sockfd");
        }
    }
    else
    {
        if (port_str == NULL)
        {
            if (client_sockfd >= 0)
            {
                retval = 1;
            }
            else
            {
                retval = 0;
            }
            // ESP_LOGE(TAG, "where are you IP ?");
            return retval;
        }
        else
        {
            tcp_client_enable = enable;
            memset(sock_ip_str,0,sizeof(sock_ip_str));
            strcpy(sock_ip_str, ip_str);
            memset(sock_port_str,0,sizeof(sock_port_str));
            strcpy(sock_port_str, port_str);
            debug_log(LOG_Info,TAG,"config link ip[%s:%s]",ip_str,sock_port_str);
            if (client_sockfd >= 0)
            {
                shutdown(client_sockfd, 0);
                close(client_sockfd);
                client_sockfd = -1;
            }
        }
    }
    if (client_sockfd >= 0)
    {
        retval = 1;
    }
    else
    {
        retval = 0;
    }
    return retval;
}

/*
    使用前确保网络通畅
*/
int tcp_client_send_data(uint8_t *data, int size)
{
    int retval = 0;
    int len = size;
    if (data == NULL || size == 0)
    {
        retval = 1;
        return retval;
    }
    
    if (client_sockfd >= 0)
    {
        send(client_sockfd, data, len, 0);
    }
    return retval;
}

/*
    接收回调函数执行指针
*/
static D_pFun tcp_client_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void tcp_client_receive_State_Machine_Bind (D_pFun Callback_pFun)
{
    tcp_client_Callback_Fun = Callback_pFun;
}

/*
    网络的应用层任务必须先确保底层网络是启动的，否则不应该启动这个任务
*/
void *tcp_client_link_task(void *empty)
{
    char host_ip[50];
    char temp_array[50];
    int ip_port = 0;
    int addr_family = 0;
    int ip_protocol = 0;
    int temp_num;
    uint8_t data[10240];

    while (1) 
    {
        debug_log(LOG_Info,TAG,"tcp_client_link_task");
        while (tcp_client_enable == 0)
        {
            sleep(1);
        }
        if (strlen(sock_port_str) <= 0)
        {
            strcpy(sock_port_str,"8160");
        }
        ip_port = atoi(sock_port_str);
        if (strlen(sock_ip_str) <= 0)
        {
            strcpy(sock_ip_str,"192.168.1.128");
        }
        debug_log(LOG_Info,TAG,"connect target ip[%s][%s],enable[%d]",sock_ip_str,sock_port_str,tcp_client_enable);
        client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(client_sockfd < 0)
        {
            perror("client socket");
        }
        struct sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = htons(ip_port);
        if(inet_pton(AF_INET, sock_ip_str, (void*)&sin.sin_addr.s_addr)<=0)
        {
            perror("client inet_pton");
            goto sock_exit;
        }
        if(connect(client_sockfd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
        {
            perror("client connect");
            goto sock_exit;
        }
        else
        {
            debug_log(LOG_Info,TAG,"connect ip[%s][%s],sock[%d],succ",sock_ip_str,sock_port_str,client_sockfd);
        }
        while (1)
        {
            if (client_sockfd >= 0)
            {
                temp_num = recv(client_sockfd, data, sizeof(data)-1,0);   // 阻塞
                if (temp_num > 0)
                {
                    if (tcp_client_Callback_Fun != NULL)
                    {
                        for (int i = 0; i < temp_num; i++)
                        {
                            tcp_client_Callback_Fun(&data[i]);  // pointer
                        }
                    }
                }
                else
                {
                    sprintf(temp_array,"client connect loss code[%d]",temp_num);
                    perror(temp_array);     // net loss
                    break;
                }
            }
            else
            {
                break;
            }
        }
        sock_exit:
        debug_log(LOG_Info,TAG,"exit sock[%d]",client_sockfd);
        if (client_sockfd >= 0)
        {
            shutdown(client_sockfd, 0);
            close(client_sockfd);
            client_sockfd = -1;
        }
        sleep(1);
    }
    tcp_client_link_config (NULL,NULL,0);
}
