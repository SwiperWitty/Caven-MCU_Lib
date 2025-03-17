/*
    前提是eth和wifi任意一个启动连接才可以使用
*/

#include "tcp_server_link.h"
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

static const char *TAG = "TCP server";

static int tcp_server_enable = 0;
static int tcp_server_sock = 0;
static char sock_port_str[10] = {0};
static char sock_flag = 0;      // 1是wifi的sock

static int server_sockfd = -1;

/*
    server 只能修改端口，如果需要修改ip请修改[eth_config_ip]/[wifi_config_ip]
    enable 0 会关闭当前sock，没有则不生效
    enable = 1,并重置ip_str,reset client
*/
int tcp_server_link_config (char *port_str,int enable)
{
    int retval = 0;
    if (enable == 0)
    {
        tcp_server_enable = enable;
        if (tcp_server_sock > 0)
        {
            shutdown(tcp_server_sock, 0);
            close(tcp_server_sock);
            tcp_server_sock = 0;
            debug_log(LOG_Warn,TAG,"close sock");
        }
        if (server_sockfd >= 0)
        {
            shutdown(server_sockfd, 0);
            close(server_sockfd);
            debug_log(LOG_Warn,TAG,"close sockfd");
        }
    }
    else
    {
        if (port_str == NULL)
        {
            retval = tcp_server_sock;
            // ESP_LOGE(TAG, "where are you IP ?");
            return retval;
        }
        else
        {
            tcp_server_enable = enable;
            memset(sock_port_str,0,sizeof(sock_port_str));
            strcpy(sock_port_str, port_str);
            debug_log(LOG_Info,TAG,"config link ip[xx.xx.xx.xx:%s]",sock_port_str);
            if (tcp_server_sock > 0)
            {
                shutdown(tcp_server_sock, 0);
                close(tcp_server_sock);
                tcp_server_sock = 0;
            }
        }
    }
    retval = tcp_server_sock;
    return retval;
}

/*
    使用前确保网络通畅
*/
int tcp_server_send_data(uint8_t *data, int size)
{
    int retval = 0;
    int len = size;
    if (data == NULL || size == 0)
    {
        retval = 1;
        return retval;
    }
    
    if (tcp_server_sock > 0)
    {
        send(tcp_server_sock, data, len, 0);
    }
    return retval;
}

/*
    接收回调函数执行指针
*/
static D_pFun tcp_server_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void tcp_server_receive_State_Machine_Bind (D_pFun Callback_pFun)
{
    tcp_server_Callback_Fun = Callback_pFun;
}

int tcp_server_Receive_Get_Fun(uint8_t* data,int size)
{
    int retval = 0;
    if (data == NULL || size == 0)
    {
        retval = -1;
        return retval;
    }
    
    if (tcp_server_sock > 0)
    {
        retval = recv(tcp_server_sock, data, size,MSG_DONTWAIT);   // 非阻塞
        if (retval < 0)
        {
            
        }
    }
    else
    {
        retval = -2;
    }
    return retval;
}

/*
    网络的应用层任务必须先确保底层网络是启动的，否则不应该启动这个任务
*/
void *tcp_server_link_task(void *empty)
{
    char cmd_str[128];
    int ip_port = 0;
    int temp_num = 0;
    int tcp_rst = 0;
    uint8_t wifi_ip_str[10];
    uint8_t wifi_gw_str[10];
    uint8_t wifi_netmask_str[10];
    uint8_t data[10240];
    GT_tcp_server_start:
    debug_log(LOG_Info,TAG,"tcp_server_link_task");
    while (tcp_server_enable == 0)
    {
        sleep(1);
    }
    if (strlen(sock_port_str) <= 0)
    {
        strcpy(sock_port_str,"8160");
    }
    ip_port = atoi(sock_port_str);
    if (tcp_rst)
    {
        if (tcp_rst > 1)
        {
            sleep(5);
        }
        tcp_rst = 0;
    }
    // 定义sockfd
    
    // 定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    memset(&server_sockaddr,0,sizeof(server_sockaddr));
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(ip_port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 建立服务器端socket 
	if((server_sockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		perror("server_sockfd creation failed");
        tcp_rst = 2;
        goto GT_tcp_server_start;
		// exit(EXIT_FAILURE);
	}
	// 设置套接字选项避免地址使用错误
	int on=1;
	if((setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)
	{
		perror("setsockopt failed");
        shutdown(server_sockfd, 0);
        close(server_sockfd);
        tcp_rst = 2;
        goto GT_tcp_server_start;
		// exit(EXIT_FAILURE);
	}
	// 将套接字绑定到服务器的网络地址上 
	if((bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr)))<0)
	{
        char temp_str[200];
        sprintf(temp_str,"server socket bind failed sock[%d][%d]",server_sockfd,server_sockaddr.sin_port);
		perror(temp_str);        // why
        shutdown(server_sockfd, 0);
        close(server_sockfd);
        tcp_rst = 2;
        goto GT_tcp_server_start;
		// exit(EXIT_FAILURE);
	}
    debug_log(LOG_Info,TAG,"Socket bound");
    // 建立监听队列listen，成功返回0，出错返回-1
    if(listen(server_sockfd,20) == -1)
    {
        perror("server listen");
        shutdown(server_sockfd, 0);
        close(server_sockfd);
        tcp_rst = 2;
        goto GT_tcp_server_start;
        // exit(1);
    }
    
    while (1)
    {
        debug_log(LOG_Info,TAG,"Socket listening");
        // 客户端套接字
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);

        debug_log(LOG_Info,TAG,"server wait link ... port[%d],enable[%d]\n",ip_port,tcp_server_enable);
        // 等待客户端连接请求到达,成功返回非负描述字，出错返回-1
        int sock = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
        if(sock<0)
        {
            perror("server connect");
            close(server_sockfd);
            goto GT_tcp_server_start;
            // exit(1);
        }
        tcp_server_sock = sock;
        debug_log(LOG_Info,TAG,"server linked !\n");
        while (1)
        {
            if (tcp_server_sock > 0)
            {
                temp_num = recv(tcp_server_sock, data, sizeof(data)-1,0);   // 阻塞
                if (temp_num > 0)
                {
                    if (tcp_server_Callback_Fun != NULL)
                    {
                        for (int i = 0; i < temp_num; i++)
                        {
                            tcp_server_Callback_Fun(&data[i]);  // pointer
                        }
                    }
                }
                else
                {
                    perror("server connect loss");     // net loss
                    break;
                }
            }
            else
            {
                break;
            }
        }
        if (sock >= 0)
        {
            shutdown(sock, 0);
            close(sock);
            tcp_server_sock = 0;
        }
        sleep(1);
    }
    tcp_server_link_config (NULL,0);
}
