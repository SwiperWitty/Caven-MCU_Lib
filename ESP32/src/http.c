#include "http.h"

#include <esp_system.h>
#include <esp_event.h>

#include <nvs_flash.h>
#include <esp_netif.h>

#include "esp_http_client.h"

#include "cJSON.h"

static char *TAG = "http_client";

static int http_enable = 0;

static char response_data[1024*4];  // 自定义缓存空间储存一次响应数据
static int recived_len = 0;         // 自定义变量储存一次响应中接收到分片数据的累计偏移

static char http_HOST[200];
static int http_port;

static char http_post_buff[1024*4];
static int http_post_len = 0;

// http客户端的事件处理回调函数
static esp_err_t http_client_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "connected to web-server");
        recived_len = 0;
        break;
    case HTTP_EVENT_ON_DATA:
        if (evt->user_data)
        {
            memcpy(evt->user_data + recived_len, evt->data, evt->data_len); // 将分片的每一片数据都复制到user_data
            recived_len += evt->data_len;   // 
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "finished a request and response!");
        recived_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "disconnected to web-server");
        recived_len = 0;
        break;
    case HTTP_EVENT_ERROR:
        ESP_LOGE(TAG, "error");
        recived_len = 0;
        break;
    default:
        break;
    }

    return ESP_OK;
}

static int http_tcpclient_create(char *url,int port)
{
    // struct sockaddr_in server_addr;
    int socket_fd = -1;
    int temp_num = 0,temp_val,temp_port = 80;
    char temp_str[200],temp_array[200],temp_ip[50];
    char *temp_pointer = NULL,*str_pointer = NULL;
    temp_val = strlen(url);
    if (url == NULL && temp_val > 0)
    {
        return socket_fd;
    }
    if (strstr(url,"http://") == NULL)
    {
        return socket_fd;
    }
    // debug_log(LOG_Info,TAG, "create url[%s] to ip",url);
    memset(temp_str,0,sizeof(temp_str));
    memset(temp_ip,0,sizeof(temp_ip));
    memset(http_HOST,0,sizeof(http_HOST));

    temp_pointer = url + strlen("http://");
    if (temp_pointer[0] < '0' || temp_pointer[0] > '9')    // Domain name
    {
        str_pointer = strchr(temp_pointer,'/');
        temp_num = str_pointer - temp_pointer;
        if (temp_num < sizeof(temp_str) && temp_num > 0)
        {
            memcpy(temp_str,temp_pointer,temp_num);
            // debug_log(LOG_Info,TAG, "source data[%s]",temp_str);
            str_pointer = strchr(temp_str,':');
            if (str_pointer != NULL)
            {
                temp_port = atoi(str_pointer+1);
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,temp_str,(str_pointer - temp_str));
                memset(temp_str,0,sizeof(temp_str));
                strcpy(temp_str,temp_array);
            }
            else
            {
                temp_port = port;
            }
            // debug_log(LOG_Info,TAG, "target data[%s]",temp_str);
            strcpy(temp_ip,temp_str);
            // strcpy(http_HOST,temp_str);
            // struct hostent *hptr;
            // if((hptr = gethostbyname(temp_str))==NULL){
            //     debug_log(LOG_Info,TAG,"gethostbyname error");
            //     return -1;
            // }
            // else
            // {
            //     if (hptr->h_addrtype == AF_INET)
            //     {
            //          memset(temp_ip,0,sizeof(temp_ip));
            //         str_pointer = inet_ntoa(*(struct in_addr*)hptr->h_addr_list[0]);
            //         strcpy(temp_ip,str_pointer);
            //     }
            //     else
            //     {
            //         debug_log(LOG_Info,TAG,"no AF_INET");
            //         return -1;
            //     }
            // }
            // debug_log(LOG_Info,TAG,"Domain name [%s:%d],ip[%s]",temp_str,temp_port,temp_ip);
        }
    }
    else
    {
        temp_pointer = url + strlen("http://");
        str_pointer = strchr(temp_pointer,'/');
        temp_num = str_pointer - temp_pointer;
        if (temp_num < sizeof(temp_str) && temp_num > 0)
        {
            memset(temp_str,0,sizeof(temp_str));
            memcpy(temp_str,temp_pointer,(str_pointer - temp_pointer));
            // debug_log(LOG_Info,TAG, "source data[%s]",temp_str);
            str_pointer = strchr(temp_str,':');
            if (str_pointer != NULL)
            {
                temp_port = atoi(str_pointer+1);
                memset(temp_array,0,sizeof(temp_array));
                memcpy(temp_array,temp_str,(str_pointer - temp_str));
                memset(temp_str,0,sizeof(temp_str));
                strcpy(temp_str,temp_array);
            }
            else
            {
                temp_port = port;
            }
            strcpy(temp_ip,temp_str);
            // strcpy(http_HOST,temp_str);
            // debug_log(LOG_Info,TAG,"create ip [%s] port [%d]",temp_str,temp_port);
        }
    }
    http_port = temp_port;
    sprintf(http_HOST,"%s:%d",temp_ip,temp_port);
    // debug_log(LOG_Info,TAG, "connect to [%s:%d]",temp_ip,temp_port);
    // http_port = temp_port;
    // if((socket_fd = socket(AF_INET,SOCK_STREAM,0))==-1){
    //     return -1;
    // }
    // server_addr.sin_family = AF_INET;
    // server_addr.sin_port = htons(temp_port);
    // server_addr.sin_addr.s_addr = inet_addr(temp_ip);

    // if(connect(socket_fd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1){
    //     return -1;
    // }
    return socket_fd;
}

int http_client_config_init (char *url_str,char *port_str,int enable)
{
    int retval = 0;
    int temp_num = 0;
    http_enable = enable;

    if (url_str == NULL || port_str == NULL)
    {
        retval = 1;
        return retval;
    }

    temp_num = strlen(url_str);
    if (temp_num)
    {
        temp_num = atoi(port_str);
        http_tcpclient_create(url_str,temp_num);
    }
    else
    {
        retval = 1;
        return retval;
    }

    return retval;
}

/*
    接收回调函数执行指针
*/
static D_Callback_pFun http_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void http_receive_Callback_Bind (D_Callback_pFun Callback_pFun)
{
    http_Callback_Fun = Callback_pFun;
}

/*
    收发本体都在其中,此函数会阻塞，请使用单独任务调用
*/
int http_port_data_Fun (char *data)
{
    esp_err_t ret;
    int temp_num = 0;
    static esp_http_client_handle_t httpclient;

    if (http_enable == 0 || data == NULL)
    {
        return (-1);
    }
    do
    {
        temp_num = Network_manage_get_status();
        if (temp_num == 0)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    } while (temp_num == 0);        // 等待网络连接
    if (temp_num)
    {
        ESP_LOGW(TAG, "get network ID [%d]",temp_num);
    }
    ESP_LOGI(TAG, "url:[%s]",http_HOST);
    esp_http_client_config_t cfg = {
        .url = http_HOST,
        .port = http_port,
        .event_handler = http_client_event_handler,
        .user_data = response_data,
    };
    // 使用http服务器配置参数对http客户端初始化
    httpclient = esp_http_client_init(&cfg);
/*
    // GET方法请求
    esp_http_client_set_method(httpclient, HTTP_METHOD_GET);
    esp_http_client_set_url(httpclient, "/add?x=10&y=11");//url传递发送数据x=10，y=11
    ret = esp_http_client_perform(httpclient);//启动并阻塞等待一次响应完成
    if (ret == ESP_OK)
    {
		//一次响应完成，打印完整响应数据，即得到X+y的值
        printf("GET:%s\n", response_data);
    }
    // POST方法请求
    esp_http_client_set_method(httpclient, HTTP_METHOD_POST);
    esp_http_client_set_url(httpclient, "/add/");
*/
    esp_http_client_set_method(httpclient, HTTP_METHOD_POST);
    esp_http_client_set_url(httpclient, http_HOST);
    // 设置请求头
    esp_http_client_set_header(httpclient, "Content-Type", "application/json");
    esp_http_client_set_header(httpclient, "User-Agent", "=ESP32 HTTP Client/1.0");
    temp_num = strlen(data);
    ret = esp_http_client_set_post_field(httpclient, data, temp_num);
    ESP_LOGW(TAG,"POST: len[%d]byte --->",temp_num);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set_post %s", esp_err_to_name(ret));
        ret = -2;
    }

    ret = esp_http_client_perform(httpclient);
    if (ret == ESP_OK)  // 响应完成
    {
        temp_num = strlen(response_data);
        ESP_LOGW(TAG,"POST: len[%d]byte <---",temp_num);
        if (http_Callback_Fun != NULL)
        {
            http_Callback_Fun (response_data);
        }
    }
    else
    {
        ret = -1;
    }
    
    if (httpclient != NULL)
    {
        esp_http_client_close(httpclient);
        esp_http_client_cleanup(httpclient); // 清空http客户端描述符
        httpclient = NULL;
    }
    return (ret);
}

/*
    一般使用这个，配合[http_cache_port_State_machine]使用
*/
void http_cache_port_data_Fun (char *data)
{
    if (data != NULL && http_enable)
    {
        int temp_num = strlen(data);
        if ((temp_num + http_post_len) < sizeof(http_post_buff))
        {
            memcpy(&http_post_buff[http_post_len],data,temp_num);
            http_post_len += temp_num;
        }
    }
}

void http_cache_clean (void)
{
    memset(http_post_buff,0,sizeof(http_post_buff));
    http_post_len = 0;
}

void http_cache_port_State_machine (void *empty)
{
    int retval = 0;
    if (http_post_len && http_enable)
    {
        retval = http_port_data_Fun (http_post_buff);
        if (retval >= 0)
        {
            http_cache_clean();
        }
    }
}
