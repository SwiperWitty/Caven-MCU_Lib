/*
    前提是eth和wifi任意一个启动连接才可以使用
*/

#include "tcp_server_link.h"

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


#define KEEPALIVE_IDLE              20
#define KEEPALIVE_INTERVAL          5
#define KEEPALIVE_COUNT             3

static const char *TAG = "TCP server";

static int tcp_server_enable = 0;
static int tcp_server_sock = 0;
static char sock_port_str[10] = {0};
static char sock_flag = 0;      // 1是wifi的sock


/*
    enable 0,会关闭当前sock，没有则不生效
    enable 1,打开server_link,并重置str,若此时str为NULL，理解为询问tcp_server_sock
    server 只能修改端口(改完请重启)，如果需要修改ip请修改[eth_config_ip]/[wifi_config_ip]
*/
int tcp_server_link_config (char *port_str,int enable)
{
    int retval = 0;
    tcp_server_enable = enable;
    if (enable == 0)
    {
        if (tcp_server_sock > 0)
        {
            ESP_LOGW(TAG, "config close sock <-- \n");
            shutdown(tcp_server_sock, 0);
            close(tcp_server_sock);
            tcp_server_sock = 0;
        }
    }
    else
    {
        if (port_str == NULL)
        {
            retval = tcp_server_sock;
            ESP_LOGW(TAG, "where are you IP ?");
            return retval;
        }
        else
        {
            memset(sock_port_str,0,sizeof(sock_port_str));
            strcpy(sock_port_str, port_str);
            ESP_LOGW(TAG, "config link ip[xx.xx.xx.xx:%s]",sock_port_str);
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
        int to_write = len;
        while (to_write > 0) 
        {
            int written = send(tcp_server_sock, data + (len - to_write), to_write, 0);
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
static D_Callback_pFun tcp_server_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void tcp_server_receive_State_Machine_Bind (D_Callback_pFun Callback_pFun)
{
    tcp_server_Callback_Fun = Callback_pFun;
}

static void do_retransmit(const int sock)
{
    int len;
    char rx_buffer[128];

    while (sock > 0) 
    {
        len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);  //  这里是阻塞的，想退出此函数kill&sock失效
        // Error occurred during receiving
        if (len < 0 || len == 0) {
            ESP_LOGE(TAG, "recv failed: errno [%d] retval:[%d]????\n", errno,len);
            tcp_server_sock = 0;
            break;
        }
        // Data received
        else {
            rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
            if (tcp_server_Callback_Fun != NULL)
            {
                for (int i = 0; i < len; i++)
                {
                    tcp_server_Callback_Fun(rx_buffer + i);
                }
            }
        }
        if (tcp_server_sock == 0)
        {
            break;
        }
    }
}

/*
    网络的应用层任务必须先确保底层网络是启动的，否则不应该启动这个任务
*/
void tcp_server_link_task(void *empty)
{
    char addr_str[128];
    int addr_family = (int)AF_INET;
    int ip_protocol = 0;
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;
    int ip_port = 0;
    int temp_num = 0;
    uint8_t wifi_ip_str[10];
    uint8_t wifi_gw_str[10];
    uint8_t wifi_netmask_str[10];
    temp_num = 0;
    do
    {
        temp_num = Network_manage_get_status();
        if (temp_num)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    } while (temp_num == 0 || tcp_server_enable == 0);        // 等待网络连接,否则不开启服务器
    // start
    if (temp_num)
    {
        ESP_LOGW(TAG, "get network rj45[1]/wifi[2] ID [%d]",temp_num);
    }
    if (strlen(sock_port_str) == 0)
    {
        strcpy(sock_port_str,"8160");
    }
    ip_port = atoi(sock_port_str);
    if (addr_family == AF_INET) {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(ip_port);
        ip_protocol = IPPROTO_IP;
    }
#ifdef CONFIG_EXAMPLE_IPV6
    else if (addr_family == AF_INET6) {
        struct sockaddr_in6 *dest_addr_ip6 = (struct sockaddr_in6 *)&dest_addr;
        bzero(&dest_addr_ip6->sin6_addr.un, sizeof(dest_addr_ip6->sin6_addr.un));
        dest_addr_ip6->sin6_family = AF_INET6;
        dest_addr_ip6->sin6_port = htons(ip_port);
        ip_protocol = IPPROTO_IPV6;
    }
#endif

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        tcp_server_sock = 0;
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#if defined(CONFIG_EXAMPLE_IPV4) && defined(CONFIG_EXAMPLE_IPV6)
    // Note that by default IPV6 binds to both protocols, it is must be disabled
    // if both protocols used at the same time (used in CI)
    setsockopt(listen_sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
#endif

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Network link, Socket start");
    ESP_LOGI(TAG, "Socket bound, open port:[%d]", ip_port);
    err = listen(listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1)
    {
        ESP_LOGI(TAG, "Socket listening");
        temp_num = 0;
        do
        {
            temp_num = Network_manage_get_status();
            if (temp_num)
            {
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
        } while (temp_num == 0 || tcp_server_enable == 0);        // 等待网络连接,否则不开启服务器
        if (temp_num)
        {
            ESP_LOGW(TAG, "get network ID [%d]",temp_num);
        }
        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        ESP_LOGI(TAG,"server wait link ... port[%d]\n",ip_port);
        int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }
        tcp_server_sock = sock;

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string
        if (source_addr.ss_family == PF_INET) {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
#ifdef CONFIG_EXAMPLE_IPV6
        else if (source_addr.ss_family == PF_INET6) {
            inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
        }
#endif
        ESP_LOGI(TAG, "Socket accepted host ip address: %s", addr_str);
        ESP_LOGI(TAG, "Socket num: [%d]", sock);
        if (wifi_get_local_ip_status(wifi_ip_str,wifi_gw_str,wifi_netmask_str) && strlen(addr_str))
        {
            uint8_t ip_temp_a,ip_temp_b,ip_temp_num = 0;
            uint8_t ip_temp_array[6];
            struct in_addr s;
            uint32_t ip4_addr_32;
            inet_pton(AF_INET, addr_str, (void *) &s);
            ip4_addr_32 = s.s_addr;
            ip_temp_array[ip_temp_num++] = (ip4_addr_32 >> (0 * 8)) & 0xff;
            ip_temp_array[ip_temp_num++] = (ip4_addr_32 >> (1 * 8)) & 0xff;
            ip_temp_array[ip_temp_num++] = (ip4_addr_32 >> (2 * 8)) & 0xff;
            ip_temp_array[ip_temp_num++] = (ip4_addr_32 >> (3 * 8)) & 0xff;
            ESP_LOGI(TAG,"wifi gateway ip %d.%d.%d.%d", wifi_gw_str[0],wifi_gw_str[1],wifi_gw_str[2],wifi_gw_str[3]);
            sock_flag = 0;
            ip_temp_num = 0;
            ip_temp_a = wifi_gw_str[ip_temp_num] & wifi_netmask_str[ip_temp_num];
            ip_temp_b = ip_temp_array[ip_temp_num] & wifi_netmask_str[ip_temp_num];
            if (ip_temp_a == ip_temp_b && ip_temp_a != 0)
            {
                ip_temp_num ++;
                ip_temp_a = wifi_gw_str[ip_temp_num] & wifi_netmask_str[ip_temp_num];
                ip_temp_b = ip_temp_array[ip_temp_num] & wifi_netmask_str[ip_temp_num];
                if (ip_temp_a == ip_temp_b)
                {
                    ip_temp_num ++;
                    ip_temp_a = wifi_gw_str[ip_temp_num] & wifi_netmask_str[ip_temp_num];
                    ip_temp_b = ip_temp_array[ip_temp_num] & wifi_netmask_str[ip_temp_num];
                    if (ip_temp_a == ip_temp_b)
                    {
                        sock_flag = 1;    // wifi
                    }
                }
            }
            if (eth_get_local_ip_status(NULL,NULL,NULL) == 0)
            {
                sock_flag = 1;
            }
            if (sock_flag == 0)
            {
                ESP_LOGW(TAG, "maybe rj45 network socket link");
            }
            else
            {
                ESP_LOGW(TAG, "maybe wifi network socket link");
            }
        }

        do_retransmit(sock);        // !!!!
        if (sock != 0)
        {
            shutdown(sock, 0);
            close(sock);
            sock = 0;
            tcp_server_sock = 0;
        }
        ESP_LOGW(TAG, "loss sock <--\n");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

CLEAN_UP:
    tcp_server_sock = 0;
    close(listen_sock);
    ESP_LOGW(TAG, "Socket kill");
    vTaskDelete(NULL);
}
