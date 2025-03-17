/*
    前提是eth和wifi任意一个启动连接才可以使用
*/

#include "tcp_client_link.h"

#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR "192.168.1.128"
#elif defined(CONFIG_EXAMPLE_IPV6)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV6_ADDR
#else
#define HOST_IP_ADDR ""
#endif

static const char *TAG = "TCP client";

static int tcp_client_enable = 0;
static int tcp_client_sock = 0;
static char sock_ip_str[50] = {0};
static char sock_port_str[10] = {0};
static char sock_flag = 0;      // 1是wifi的sock


/*
    enable 0,会关闭当前sock，没有则不生效
    enable 1,打开tcp_client_sock,并重置str,若此时str为NULL，理解为询问tcp_client_sock
*/
int tcp_client_link_config (char *ip_str,char *port_str,int enable)
{
    int retval = 0;
    tcp_client_enable = enable;
    if (enable == 0)
    {
        if (tcp_client_sock > 0)
        {
            ESP_LOGW(TAG, "config client ");
            shutdown(tcp_client_sock, 0);
            close(tcp_client_sock);
            tcp_client_sock = 0;
        }
    }
    else
    {
        if (ip_str == NULL || port_str == NULL)
        {
            retval = tcp_client_sock;
            ESP_LOGE(TAG, "where are you IP ?");
            return retval;
        }
        else
        {
            memset(sock_ip_str,0,sizeof(sock_ip_str));
            memset(sock_port_str,0,sizeof(sock_port_str));
            strcpy(sock_ip_str, ip_str);
            strcpy(sock_port_str, port_str);
            ESP_LOGW(TAG, "config link ip[%s:%s]",sock_ip_str,sock_port_str);
            if (tcp_client_sock > 0)
            {
                ESP_LOGW(TAG, "ip change config close sock <-- \n");
                shutdown(tcp_client_sock, 0);
                close(tcp_client_sock);
                tcp_client_sock = 0;
            }
        }
    }
    retval = tcp_client_sock;
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
    
    if (tcp_client_sock > 0)
    {
        int to_write = len;
        while (to_write > 0) 
        {
            int written = send(tcp_client_sock, data + (len - to_write), to_write, 0);
            if (written < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            }
            to_write -= written;
        }
    }
    return retval;
}

/*
    接收回调函数执行指针
*/
static D_Callback_pFun tcp_client_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void tcp_client_receive_State_Machine_Bind (D_Callback_pFun Callback_pFun)
{
    tcp_client_Callback_Fun = Callback_pFun;
}

static void do_retransmit(const int sock)
{
    int len;
    char rx_buffer[128];

    while (sock > 0) 
    {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        // Error occurred during receiving
        if (len < 0 || len == 0) {
            ESP_LOGE(TAG, "recv failed: errno [%d] retval:[%d]????\n", errno,len);
            tcp_client_sock = 0;
            break;
        }
        // Data received
        else {
            rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
            if (tcp_client_Callback_Fun != NULL)
            {
                for (int i = 0; i < len; i++)
                {
                    tcp_client_Callback_Fun(rx_buffer + i);
                }
            }
        }
        if (tcp_client_sock == 0)
        {
            break;
        }
    }
}

/*
    网络的应用层任务必须先确保底层网络是启动的，否则不应该启动这个任务
*/
void tcp_client_link_task(void *empty)
{
    char host_ip[50];
    int addr_family = 0;
    int ip_protocol = 0;
    int ip_port;
    int temp_num;
    while (1) 
    {
        temp_num = 0;
        do
        {
            temp_num = Network_manage_get_status();
            if (temp_num)
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        } while (temp_num == 0 || tcp_client_enable == 0);  // 等待网络连接,否则不开启服务器
        while (strlen(sock_ip_str) == 0)                    // 等待目标ip
        {
            ESP_LOGW(TAG, "target ip null,wait...");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        if (temp_num)
        {
            ESP_LOGW(TAG, "get network rj45[1]/wifi[2] ID [%d]",temp_num);
        }
        if (strlen(sock_port_str) == 0)
        {
            strcpy(sock_port_str,"9090");
        }
        sock_flag = 0;
        ip_port = atoi(sock_port_str);
        strcpy(host_ip,sock_ip_str);
#if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(host_ip);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(ip_port);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_IPV6)
        struct sockaddr_in6 dest_addr = { 0 };
        inet6_aton(host_ip, &dest_addr.sin6_addr);
        dest_addr.sin6_family = AF_INET6;
        dest_addr.sin6_port = htons(ip_port);
        dest_addr.sin6_scope_id = esp_netif_get_netif_impl_index(EXAMPLE_INTERFACE);
        addr_family = AF_INET6;
        ip_protocol = IPPROTO_IPV6;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(ip_port, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
#endif
        tcp_client_sock = -1;
        ESP_LOGI(TAG, "Socket created,try connecting to %s:%d ...", host_ip, ip_port);
        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            goto CLEAN_UP;
        }
        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            goto CLEAN_UP;
        }
        ESP_LOGI(TAG, "Successfully connected");
        ESP_LOGI(TAG, "Socket num: [%d]", sock);
        tcp_client_sock = sock;

        do_retransmit(sock);        // !!!!
CLEAN_UP :
        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
            sock = 0;
            tcp_client_sock = 0;
        }
        if (sock > 0)
        {
            shutdown(sock, 0);
            close(sock);
            sock = 0;
            tcp_client_sock = 0;
        }
        ESP_LOGW(TAG, "loss sock <--\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
    tcp_client_sock = 0;
    vTaskDelete(NULL);
}
