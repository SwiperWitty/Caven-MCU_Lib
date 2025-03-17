#include "http.h"
#include "debug_log.h"

#define HTTP_POST_METHOD    "POST / HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n" \
    "Content-Type:application/json\r\nContent-Length: %d\r\n\r\n%s"
#define HTTP_GET_METHOD     "GET  / HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n\r\n"

static char *TAG = "http_client";

static int http_enable = 0;

static char response_data[4096];    // 自定义缓存空间储存一次响应数据
static int recived_len = 0;         // 自定义变量储存一次响应中接收到分片数据的累计偏移

static char http_url_str[100];
static char http_port_str[20];
static uint32_t http_port_value = 9090;

static char http_send_buff[540000];
static char http_post_buff[409600];
static int http_post_len = 0;

static int http_socket_fd = -1;

// http客户端的事件处理回调函数

// http config
int http_client_config_init (char *url_str,char *port_str,int enable)
{
    int retval = 0;
    int temp_num = 0;
    int temp_run = 0;
    char temp_str[300];
    http_enable = enable;

    if (http_socket_fd > 0)
    {
        close(http_socket_fd);
    }
    
    if (url_str == NULL)
    {
        retval = 1;
        return retval;
    }
    memset(temp_str,0,sizeof(temp_str));

    temp_num = strlen(url_str);
    if (temp_num > 0 && temp_num < sizeof(temp_str))
    {
        memset(http_url_str,0,sizeof(http_url_str));
        memcpy(http_url_str,url_str,temp_num);
        memcpy(&temp_str[temp_run],http_url_str,temp_num);
        temp_run += temp_num;
    }
    else
    {
        retval = 1;
        return retval;
    }
    if (port_str != NULL)
    {
        temp_num = strlen(port_str);
        if (temp_num && temp_num < sizeof(http_port_str))
        {
            memset(http_port_str,0,sizeof(http_port_str));
            memcpy(http_port_str,port_str,temp_num);
        }
    }
    else
    {
        memset(http_port_str,0,sizeof(http_port_str));
        strcpy(http_port_str,"8080");
        temp_num = strlen(http_port_str);
    }
    http_port_value = atoi(http_port_str);
    debug_log(LOG_Info,TAG,"config %s:%d [%d]",http_url_str,http_port_value,http_enable);
    if (retval == 0)
    {
        
    }
    return retval;
}

/*
    接收回调函数执行指针
*/
static D_pFun http_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void http_receive_Callback_Bind (D_pFun Callback_pFun)
{
    http_Callback_Fun = Callback_pFun;
}

static char http_HOST[50];
static int http_port;
static int http_tcpclient_create(char *url)
{
    struct sockaddr_in server_addr;
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
                temp_port = 8080;
            }
            debug_log(LOG_Info,TAG, "target data[%s]",temp_str);
            strcpy(http_HOST,temp_str);
            struct hostent *hptr;
            if((hptr = gethostbyname(temp_str))==NULL){
                debug_log(LOG_Info,TAG,"gethostbyname error");
                return -1;
            }
            else
            {
                if (hptr->h_addrtype == AF_INET)
                {
                    str_pointer = inet_ntoa(*(struct in_addr*)hptr->h_addr_list[0]);
                    strcpy(temp_ip,str_pointer);
                }
                else
                {
                    debug_log(LOG_Info,TAG,"no AF_INET");
                    return -1;
                }
            }
            debug_log(LOG_Info,TAG,"Domain name [%s:%d],ip[%s]",temp_str,temp_port,temp_ip);
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
                temp_port = 8080;
            }
            strcpy(http_HOST,temp_str);
            strcpy(temp_ip,temp_str);
            // debug_log(LOG_Info,TAG,"create ip [%s] port [%d]",temp_str,temp_port);
        }
    }
    debug_log(LOG_Info,TAG, "connect to [%s:%d]",temp_ip,temp_port);
    http_port = temp_port;
    if((socket_fd = socket(AF_INET,SOCK_STREAM,0))==-1){
        return -1;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(temp_port);
    server_addr.sin_addr.s_addr = inet_addr(temp_ip);

    if(connect(socket_fd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1){
        return -1;
    }
    return socket_fd;
}

/*
    收发本体都在其中,此函数会阻塞，请使用单独任务调用
*/
int http_port_data_Fun (char *data)
{
    int retval = 0;
    int temp_num = 0;

    if (http_url_str == NULL || data == NULL)
    {
        return (-1);
    }

    debug_log(LOG_Info,TAG, "url:[%s]",http_url_str);
    temp_num = memcmp(http_url_str,"http://",strlen("http://"));
    if (temp_num != 0)
    {
        debug_log(LOG_Error,TAG, "http url type error[%d]",temp_num);
        return (-2);
    }
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
    memset(http_send_buff,0,sizeof(http_send_buff));
    memset(response_data,0,sizeof(response_data));
    
    http_socket_fd = http_tcpclient_create(http_url_str);
    if(http_socket_fd < 0){
        debug_log(LOG_Error,TAG,"http_tcpclient create failed");
        return retval = -3;
    }
    sprintf(http_send_buff,HTTP_POST_METHOD,http_HOST,http_port,strlen(data),data);

    retval = send(http_socket_fd,http_send_buff,strlen(http_send_buff),0);
    if(retval < 0)
    {
        debug_log(LOG_Error,TAG,"send failed..");
        return retval;
    }
    temp_num = recv(http_socket_fd, response_data,sizeof(response_data)-1,MSG_DONTWAIT);
    if(temp_num <= 0){
        sleep(2);
        temp_num = recv(http_socket_fd, response_data,sizeof(response_data)-1,MSG_DONTWAIT);
    }
    /*it's time to recv from server*/
    if(temp_num <= 0){
        debug_log(LOG_Warn,TAG,"recv failed");
        retval = 1;
    }
    else
    {
        debug_log(LOG_Info,TAG,"POST: len[%d]byte <---",temp_num);
        if (http_Callback_Fun != NULL)
        {
            http_Callback_Fun (response_data);
        }
        retval = 0;
    }
    
    // debug_log(LOG_Info,TAG,"http over");
    if (http_socket_fd >= 0)
    {
        close(http_socket_fd);
    }
    return (retval);
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
    if (http_post_len > 0 && http_enable != 0)
    {
        retval = http_port_data_Fun (http_post_buff);
        if (retval >= 0)
        {
            http_cache_clean();
        }
    }
}
