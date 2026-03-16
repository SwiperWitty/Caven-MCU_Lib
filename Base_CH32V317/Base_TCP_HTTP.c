#include "Base_TCP_HTTP.h"
#include "Base_ETH.h"
#include "Base_TCP_Client.h"
#include "Base_Sys_Time.h"
#include "Time_Handle.h"
#include "Data_Handle.h"

#define HTTP_POST_METHOD    "POST / HTTP/1.1\r\nHOST: %s:%s\r\nAccept: */*\r\n" \
    "Content-Type:application/json\r\nContent-Length: %d\r\n\r\n%s"

#if Exist_ETH
static u8 HTTP_init_flag = 0;

static char client_ip[40];
static char client_port[40];
static char client_send_cache[1024];
static int cache_run = 0;
static char client_get_cache[1024];
static int cache_get = 0,cache_read = 0;

static void Base_TCP_HTTP_GET_Fun (void *data)
{
    #if Exist_ETH
    char temp_data = *(char *)data;
    if(cache_get < sizeof(client_get_cache))
    {
        client_get_cache[cache_get ++] = temp_data;
    }
    #endif
}

#endif

int Base_TCP_HTTP_Config (char *http,int enable)
{
    int retval = 0;
    #if Exist_ETH
    if(enable)
    {
        int temp_num;
        char array_url[200],array_path[100];
        memset(array_url,0,sizeof(array_url));
        memset(array_path,0,sizeof(array_path));
        memset(client_ip,0,sizeof(client_ip));
        memset(client_port,0,sizeof(client_port));
        temp_num = Caven_http_To_url (http,array_url,array_path);
        if(temp_num)
        {
            temp_num = Base_ETH_IPprot (array_url,client_ip,client_port);
        }
        else
        {
            printf("HTTP_Config:http error\r\n");
        }
        if (temp_num)
        {
            Base_ETH_HTTP_pFun_Bind (Base_TCP_HTTP_Task);
            Base_TCP_Client_Receive_Bind_Fun (Base_TCP_HTTP_GET_Fun);
            Base_TCP_Client_Config (client_ip,client_port,enable);
            HTTP_init_flag = 1;
            printf("HTTP_Config url:%s:%s/%s,running... \r\n",client_ip,client_port,array_path);
        }
        else
        {
            printf("HTTP_Config:url to IPprot error\r\n");
        }
    }
    else
    {
        Base_TCP_Client_Config (NULL,NULL,0);
        HTTP_init_flag = 0;
    }
    #endif
    return retval;
}

int Base_TCP_HTTP_cache_Send_Fun (char *data, int len)
{
    int retval = 0;
    #if Exist_ETH
    if (data != NULL && HTTP_init_flag)
    {
        if ((len + cache_run) < sizeof(client_send_cache))
        {
            memcpy(&client_send_cache[cache_run],data,len);
            cache_run += len;
            client_send_cache[cache_run] = 0;
        }
    }
    #endif
    return retval;
}

int Base_TCP_HTTP_cache_Read_Fun (char *data,int len_max)
{
    int retval = 0;
    #if Exist_ETH
    if(data != NULL && len_max > 0 && cache_get)
    {
        retval = cache_get;
        if(len_max < cache_get)
        {
            retval = len_max;
        }
        cache_get = 0;
        memcpy(data,client_get_cache,retval);
        cache_read = 1;
    }
    #endif
    return retval;
}

void Base_TCP_HTTP_Task (u8 noway,u8 nobady)
{
    #if Exist_ETH
    static int http_run = 0,resend = 0;
    static Task_Overtime_Type http_task = {
        .Begin_time = {0},
        .Set_time.SYS_Sec = 1,
        .Set_time.SYS_Us = 0,
        .Switch = 1,
    };
    SYS_BaseTIME_Type base_time;
    Caven_BaseTIME_Type http_time;
    int temp_num = 0;
    char http_send_buff[2048];
    if(HTTP_init_flag == 0)
    {
        return;
    }
    if(cache_run || http_run)
    {
        switch (http_run) 
        {
            case 0:     // client link
            {
                Base_TCP_Client_Config (NULL,NULL,0);
                Base_TCP_Client_Config (client_ip,client_port,1);
                http_run ++;
            }
            break;
            case 1:     // send data
            {
                if(Base_TCP_Client_Config (NULL,NULL,1) == 1)
                {
                    sprintf(http_send_buff,HTTP_POST_METHOD,client_ip,client_port,strlen(client_send_cache),client_send_cache);
                    temp_num = Base_TCP_Client_Send ((uint8_t *)http_send_buff, strlen(http_send_buff));
                    if(temp_num == 0)
                    {
                        http_run ++;
                        // printf("HTTP_Run post over \r\n");
                        SYS_Time_Get(&base_time);
                        http_time.SYS_Sec = base_time.SYS_Sec;
                        http_time.SYS_Us = base_time.SYS_Us;
                        http_task.Begin_time = http_time;
                        http_task.Trigger_Flag = 0;
                    }
                }
            }
            break;
            case 2:     // wait Receive
            {
                SYS_Time_Get(&base_time);
                http_time.SYS_Sec = base_time.SYS_Sec;
                http_time.SYS_Us = base_time.SYS_Us;
                API_Task_Timer (&http_task,http_time);
                if(cache_get > 0 || cache_read)     // wait Receive succ
                {
                    http_run  = 3;
                }
                else
                {
                    if(http_task.Trigger_Flag)      // wait Receive overtime
                    {
                        http_run = 0;
                        resend ++;
                        // printf("HTTP_Run over time try again !\r\n");
                        if(resend >= 2)
                        {
                            http_run = 0xff;
                            resend = 0;
                        }
                    }
                }
            }
            case 3:
            {
                if(cache_read)      // app read over
                {
                    // printf("HTTP_Run get over \r\n");
                    http_run = 0xff;
                }
            }
            break;
            default:
            {
                // printf("HTTP_Run task over \r\n");
                Base_TCP_Client_Config (NULL,NULL,0);
                http_task.Trigger_Flag = 0;
                http_run = 0;
                cache_run = 0;
                cache_get = 0;
                cache_read = 0;
            }
            break;
        }
    }
    #endif
}

