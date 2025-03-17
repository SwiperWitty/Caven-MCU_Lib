#include "https.h"

#include "esp_event.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_http_client.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "cJSON.h"

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
#include "esp_crt_bundle.h"
#endif


//Constants that aren't configurable in menuconfig
//
#define body   "{\"eventType\":\"ObjectEvent\",\"action\":\"OBSERVE\",\"eventTime\":\"2024-04-11T10:47:29.066Z\",\"identifiers\":[\"303431711C5B07C000002714\"],\"readPoint\":\"SN-123123\",\"businessLocation\":\"urn:atma.io:loc:rovinj:rovinj:9018\",\"businessStep\":\"urn:epcglobal:cbv:bizstep:inspecting\",\"disposition\":\"urn:epcglobal:cbv:disp:conformant\",\"extensionElements\":{\"TID\":\"E2082734893739367282\",\"Session Scan\":\"1\",\"Session Action\":\"Normal\"}}"
/*
static const char HOWSMYSSL_REQUEST[1024] = "POST " WEB_URL " HTTP/1.1\r\n"
                             "Host: "WEB_SERVER"\r\n"
                             "User-Agent: esp-idf/1.0 esp32\r\n"
                             "Content-Type: application/json\r\n"
                             "Subscription-Key: d31b82fdd20d4d019dc0f014447384f8\r\n"
                             "Content-Length: 408\r\n"
                             "\r\n"
                             body
                             "\r\n";
*/
#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
static const char LOCAL_SRV_REQUEST[] = "GET " CONFIG_EXAMPLE_LOCAL_SERVER_URL " HTTP/1.1\r\n"
                             "Host: "WEB_SERVER"\r\n"
                             "User-Agent: esp-idf/1.0 esp32\r\n"
                             "\r\n";
#endif

#define CONFIG_MBEDTLS_CERTIFICATE_BUNDLE 1
#define CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_FULL 1
#define CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_MAX_CERTS 200

#define SERVER_URL_MAX_SZ 256

static const char *TAG = "https";

static int https_enable = 0;

static char https_Host_str[100] = {0};
static char https_url_str[100] = {0};
static char https_port_str[10] = {"443"};

static char https_head_str[300];
static int https_head_len = 0;

static char https_post_buff[1024*4];
static int https_post_len = 0;

static char https_send_buff[1024];

/*
    接收回调函数执行指针
*/
static D_Callback_pFun https_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void https_receive_Callback_Bind (D_Callback_pFun Callback_pFun)
{
    https_Callback_Fun = Callback_pFun;
}

/* Root cert for howsmyssl.com, taken from server_root_cert.pem

   The PEM file was extracted from the output of this command:
   openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null

   The CA root cert is the last cert given in the chain of certs.

   To embed it in the app binary, the PEM file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

extern const uint8_t local_server_cert_pem_start[] asm("_binary_local_server_cert_pem_start");
extern const uint8_t local_server_cert_pem_end[]   asm("_binary_local_server_cert_pem_end");

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
static esp_tls_client_session_t *tls_client_session = NULL;
static bool save_client_session = false;
#endif

static void https_get_request(esp_tls_cfg_t cfg, const char *WEB_SERVER_URL, const char *REQUEST)
{
    char buf[512];
    int ret = 0, len;
    char succ_str[30] = "202 Accepted";
    char* succ_flag = NULL;
    /*1.建立https连接*/
    struct esp_tls *tls = esp_tls_conn_http_new(WEB_SERVER_URL, &cfg);

    if (tls != NULL) {
        ESP_LOGI(TAG, "Connection established...");
    } else {
        ESP_LOGE(TAG, "Connection failed...");
        goto exit;
    }

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
    /* The TLS session is successfully established, now saving the session ctx for reuse */
    if (save_client_session) {
        free(tls_client_session);
        tls_client_session = esp_tls_get_client_session(tls);
    }
#endif
    size_t written_bytes = 0;
    /*2.写入http请求*/
    ESP_LOGW(TAG, "send to [%s],len [%d] --->",WEB_SERVER_URL,strlen(REQUEST));
    do {
        ret = esp_tls_conn_write(tls,
                                 REQUEST + written_bytes,
                                 strlen(REQUEST) - written_bytes);
        if (ret >= 0) {
            ESP_LOGI(TAG, "%d bytes written", ret);
            written_bytes += ret;
        } else if (ret != ESP_TLS_ERR_SSL_WANT_READ  && ret != ESP_TLS_ERR_SSL_WANT_WRITE) {
            ESP_LOGE(TAG, "esp_tls_conn_write  returned: [0x%02X](%s)", ret, esp_err_to_name(ret));
            goto exit;
        }
    } while (written_bytes < strlen(REQUEST));

    ESP_LOGI(TAG, "Reading HTTP response...");
    /*3.等待服务器回应*/
    do {
        len = sizeof(buf) - 1;
        bzero(buf, sizeof(buf));
        ret = esp_tls_conn_read(tls, (char *)buf, len);

        if (ret == ESP_TLS_ERR_SSL_WANT_WRITE  || ret == ESP_TLS_ERR_SSL_WANT_READ) {
            if (succ_flag != NULL)
            {
                ESP_LOGW(TAG, "get HTTP succ");
                goto exit;
            }
            continue;
        }

        if (ret < 0) {
            ESP_LOGE(TAG, "esp_tls_conn_read  returned [-0x%02X](%s)", -ret, esp_err_to_name(ret));
            break;
        }

        if (ret == 0) {
            ESP_LOGI(TAG, "connection closed !");
            // 完成
            break;
        }
        if (ret < len)
        {
            len = ret;
        }
        
        /* Print response directly to stdout as it is read */
        if (len)
        {
            ESP_LOGW(TAG,"POST: len[%d] byte <---",len);
            if (https_Callback_Fun != NULL)
            {
                https_Callback_Fun(buf);
            }
            succ_flag = strstr(buf, succ_str);
            // JSON output doesn't have a newline at end
        }
        if (succ_flag != NULL)
        {
            goto exit;
        }
    } while (1);

exit:
    if(ret != 0)
    {
        ESP_LOGI(TAG, "connection closed !!");
    }
    esp_tls_conn_delete(tls);
    vTaskDelay(10 / portTICK_PERIOD_MS);
}

#if CONFIG_MBEDTLS_CERTIFICATE_BUNDLE
static void https_get_request_using_crt_bundle(void)
{
    ESP_LOGI(TAG, "https_request using crt bundle");
    /*启动根证书捆绑包，并指定捆绑软件附加功能*/
    esp_tls_cfg_t cfg = {
        .crt_bundle_attach = esp_crt_bundle_attach,
    };
    https_get_request(cfg, https_url_str, https_send_buff);
}
#endif // CONFIG_MBEDTLS_CERTIFICATE_BUNDLE

static void https_get_request_using_cacert_buf(void)
{
    ESP_LOGI(TAG, "https_request using cacert_buf");
    /*
    	1.指定cacert_buf缓冲区中的pem二进制文件
    	2.cacert_bytes为该二进制文件大小
    */
    esp_tls_cfg_t cfg = {
        .cacert_buf = (const unsigned char *) server_root_cert_pem_start,
        .cacert_bytes = server_root_cert_pem_end - server_root_cert_pem_start,
    };
    https_get_request(cfg, https_url_str, https_send_buff);
}

static void https_get_request_using_global_ca_store(void)
{
    esp_err_t esp_ret = ESP_FAIL;
    ESP_LOGI(TAG, "https_request using global ca_store");
    /*使用wolfssl设置全局CA存储数据TLS/SSL*/
    esp_ret = esp_tls_set_global_ca_store(server_root_cert_pem_start, server_root_cert_pem_end - server_root_cert_pem_start);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error in setting the global ca store: [%02X] (%s),could not complete the https_request using global_ca_store", esp_ret, esp_err_to_name(esp_ret));
        return;
    }
    /*所有连接使用全局ca_store*/
    esp_tls_cfg_t cfg = {
        .use_global_ca_store = true,
    };
    https_get_request(cfg, https_url_str, https_send_buff);
    /*释放全局ca_store*/
    esp_tls_free_global_ca_store();
}

#ifdef CONFIG_EXAMPLE_CLIENT_SESSION_TICKETS
static void https_get_request_to_local_server(const char* url)
{
    ESP_LOGI(TAG, "https_request to local server");
    esp_tls_cfg_t cfg = {
        .cacert_buf = (const unsigned char *) local_server_cert_pem_start,
        .cacert_bytes = local_server_cert_pem_end - local_server_cert_pem_start,
        .skip_common_name = true,
    };
    save_client_session = true;
    https_get_request(cfg, url, LOCAL_SRV_REQUEST);
}

static void https_get_request_using_already_saved_session(const char *url)
{
    ESP_LOGI(TAG, "https_request using saved client session");
    esp_tls_cfg_t cfg = {
        .client_session = tls_client_session,
    };
    https_get_request(cfg, url, LOCAL_SRV_REQUEST);
    esp_tls_free_client_session(tls_client_session);
    save_client_session = false;
    tls_client_session = NULL;
}
#endif

/*
    初始化函数，应该在任务前面
*/
int https_request_config_init (char *way_str,char *Host_str,char *URL_str,char *port_str,int enable)
{
    int retval = 0;
    int temp_num = 0;
    int temp_run = 0;
    char temp_str[300];
    https_enable = enable;
    if (way_str == NULL || Host_str == NULL || URL_str == NULL)
    {
        retval = 1;
        return retval;
    }
    memset(temp_str,0,sizeof(temp_str));
    temp_num = strlen(way_str);
    if (temp_num)
    {
        memcpy(&temp_str[temp_run],way_str,temp_num);
        temp_run += temp_num;
        memcpy(&temp_str[temp_run]," ",1);
        temp_run += 1;
    }
    else
    {
        retval = 1;
        return retval;
    }
    temp_num = strlen(URL_str);
    if (temp_num)
    {
        memset(https_url_str,0,sizeof(https_url_str));
        memcpy(https_url_str,URL_str,temp_num);
        memcpy(&temp_str[temp_run],URL_str,temp_num);
        temp_run += temp_num;
        temp_num = strlen(" HTTP/1.1\r\n");
        memcpy(&temp_str[temp_run]," HTTP/1.1\r\n",temp_num);       // 请求行
        temp_run += temp_num;
    }
    else
    {
        retval = 1;
        return retval;
    }
    temp_num = strlen(Host_str);
    if (temp_num)
    {
        memset(https_Host_str,0,sizeof(https_Host_str));
        memcpy(https_Host_str,Host_str,temp_num);

        memset(https_head_str,0,sizeof(https_head_str));
        memcpy(https_head_str,temp_str,temp_run);
        https_head_len = temp_run;

        https_request_add_header ("Host",Host_str);
        temp_run = https_head_len;
        https_request_add_header ("User-Agent","esp-idf/1.0 esp32");
        temp_run = https_head_len;
        https_request_add_header ("Content-Type","application/json");
        temp_run = https_head_len;
    }
    else
    {
        retval = 1;
        return retval;
    }
    if (port_str != NULL)
    {
        temp_num = strlen(port_str);
        if (temp_num && temp_num < sizeof(https_port_str))
        {
            memset(https_port_str,0,sizeof(https_port_str));
            memcpy(https_port_str,port_str,temp_num);
            ESP_LOGI(TAG, "port:[%s]",port_str);
        }
    }
    if (retval == 0)
    {
        // ESP_LOGI(TAG, " \n%s",https_head_str);
    }
    return retval;
}
/*
    给header添加一些type
*/
int https_request_add_header (char *type,char *data)
{
    int retval = 0;
    int temp_num = 0;
    if (type == NULL || data == NULL)
    {
        ESP_LOGE(TAG, "where are you data ?");
        retval = 1;
    }
    else if (https_head_len == 0)
    {
        ESP_LOGE(TAG, "where are you header ?");
        retval = 2;
    }
    else
    {
        retval = https_head_len;
        temp_num = strlen(type);
        memcpy(&https_head_str[https_head_len],type,temp_num);
        https_head_len += temp_num;

        temp_num = strlen(": ");
        memcpy(&https_head_str[https_head_len],": ",temp_num);
        https_head_len += temp_num;

        temp_num = strlen(data);
        memcpy(&https_head_str[https_head_len],data,temp_num);
        https_head_len += temp_num;

        temp_num = strlen("\r\n");
        memcpy(&https_head_str[https_head_len],"\r\n",temp_num);
        https_head_len += temp_num;

        temp_num = strlen("\r\n");
        memcpy(&https_head_str[https_head_len],"\r\n",temp_num);

        retval = 0;
    }
    return retval;
}

/*
    添加数据长度体
    将data_str融入https_send_buff
    给body加上末尾
    按照type发送
    此函数会阻塞，请使用单独任务调用
*/
int https_request_data_Fun (char type,char *data)
{
    int retval = 1;
    int temp_run = 0;
    int temp_num = 0;
    char temp_str[20];

    if (https_head_len == 0)
    {
        ESP_LOGE(TAG, "where are you header ?");
        return retval;
    }
    else if (strlen(https_url_str) == 0)
    {
        ESP_LOGE(TAG, "where are you url ?");
        return retval;
    }
    else if (data == NULL)
    {
        ESP_LOGE(TAG, "where are you data ?");
        return retval;
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
    retval = https_head_len;
    temp_num = strlen(data);
    if (temp_num == 0 || temp_num > 800)
    {
        ESP_LOGE(TAG, "data len error");
        return retval;
    }
    memset(temp_str,0,sizeof(temp_str));
    sprintf(temp_str,"%d",temp_num);
    https_request_add_header ("Content-Length",temp_str);
    temp_run = https_head_len + 2;  // 把header结束符"\r\n"算进去
    https_head_len = retval;        // 还原https_head_len，不能将Content算入头长度中，否则下次头长度会叠加
    
    memcpy(&https_send_buff[0],https_head_str,temp_run);    // 将头载入
    memcpy(&https_send_buff[temp_run],data,temp_num);       // 将body载入
    temp_run += temp_num;
    temp_num = strlen("\r\n");
    memcpy(&https_send_buff[temp_run],"\r\n",temp_num);     // 数据结束
    temp_run += temp_num;
    https_send_buff[temp_run] = 0;  // 切断

    retval = 0;
    switch (type)
    {
    case 1:
        https_get_request_using_crt_bundle();
        break;
    case 2:
        https_get_request_using_cacert_buf();
        break;
    case 3:
        https_get_request_using_global_ca_store();
        break;
    default:
        https_get_request_using_crt_bundle();
        break;
    }
    return retval;
}

/*
    这个函数是user级函数(自定义内容的，本质是咋写都行)
*/
void JSON_DataToAtmajson (int utc,char *epc,char *tid,char *sn,char *str,char num,char state,char *ret_data)
{
    char temp_str[30];
    cJSON *all_data_item = cJSON_CreateObject();
    cJSON *tags = cJSON_CreateObject();
    cJSON *array = cJSON_CreateArray();
    if (all_data_item != NULL)
    {
        char *out = 0;
        int add_flag = 0;
        //
        cJSON_AddStringToObject(all_data_item, "eventType", "ObjectEvent");
        cJSON_AddStringToObject(all_data_item, "action", "OBSERVE");
        cJSON_AddStringToObject(all_data_item, "eventTime", "2024-04-11T10:47:29.066Z");    // 时间
        
        cJSON_AddItemToArray(array, cJSON_CreateString(epc));    // epc
        cJSON_AddItemToObject(all_data_item, "identifiers", array);

        cJSON_AddStringToObject(all_data_item, "readPoint", sn);            // 序列号
        cJSON_AddStringToObject(all_data_item, "businessLocation", str);    // 合成的拼接
        cJSON_AddStringToObject(all_data_item, "businessStep", "urn:epcglobal:cbv:bizstep:inspecting");
        cJSON_AddStringToObject(all_data_item, "disposition", "urn:epcglobal:cbv:disp:conformant");
        if (tags != NULL)
        {
            // cJSON_AddNullToObject(tags, "TID");
            cJSON_AddStringToObject(tags, "TID", tid);       // tid
            memset(temp_str,0,sizeof(temp_str));
            sprintf(temp_str,"%d",num);
            cJSON_AddStringToObject(tags, "Session Scan", temp_str);                 // 编号
            memset(temp_str,0,sizeof(temp_str));
            switch (state)
            {
            case 1:
                sprintf(temp_str,"%s","Normal");
                break;
            case 2:
                sprintf(temp_str,"%s","Duplicate");
                break;
            case 3:
                sprintf(temp_str,"%s","Multi");
                break;
            case 4:
                sprintf(temp_str,"%s","Encoding Issue");
                break;
            default:
                break;
            }
            cJSON_AddStringToObject(tags, "Session Action", temp_str);          // 状态
            cJSON_AddItemToObject(all_data_item, "extensionElements", tags);
            add_flag ++;
        }
        //
        out = cJSON_PrintUnformatted(all_data_item);
        // ESP_LOGI(TAG, "HTTP JSON Pack data :\n%s", out);
        
        if (out != NULL)
        {
            strcpy(ret_data,out);
            free(out);
        }
        if (add_flag)       // cJSON_AddItemToObject 之后不要释放子集
        {
            cJSON_Delete(all_data_item);
        }
        else
        {
            cJSON_Delete(tags);
            cJSON_Delete(all_data_item);
        }
    }
}

char https_type = 0;
/*
    一般使用这个，配合[https_cache_port_State_machine]使用
*/
void https_cache_request_data_Fun (char type,char *data)
{
    if (data != NULL && https_enable)
    {
        https_type = type;
        int temp_num = strlen(data);
        if ((temp_num + https_post_len) < sizeof(https_post_buff))
        {
            memcpy(&https_post_buff[https_post_len],data,temp_num);
            https_post_len += temp_num;
        }
    }
}

void https_cache_clean (void)
{
    memset(https_post_buff,0,sizeof(https_post_buff));
    https_post_len = 0;
}

void https_cache_request_State_machine (void *empty)
{
    if (https_post_len && https_enable)
    {
        https_request_data_Fun (https_type,https_post_buff);
        https_cache_clean();
    }
}
/*
    网络的应用层任务必须先确保底层网络是启动的，否则不应该启动这个任务
    HTTPS的逻辑：
    先对框架进行设置和搭建添加报头[https_request_config_init],[https_request_add_header]
    然后把要发送的东西给[JSON_DataToAtmajson]自动生成user格式,由[https_request_data_Fun]自动发送出去(使用这个任务就是为了减少发送次数，缓存发送)
    数据的返回在[esp_tls_conn_read]这个位置实现
    HTTPS是无法在本地模拟的，只能去服务器看，但是想要debug看发送内容，直接在[JSON_DataToAtmajson]中打印即可。
*/
void eps32_HTTPS_task (void *empty)
{
    int times = 0;
    int temp_num = 0;
    char http_array[1024];
    
    https_request_add_header ("Subscription-Key","d31b82fdd20d4d019dc0f014447384f8");
    https_request_add_header ("12345","67890");

    ESP_LOGI(TAG, "https_head \n[%s]",https_head_str);

    // ESP_LOGI(TAG, "way 1");
    // https_request_data_Fun (1,body);
    // ESP_LOGI(TAG, "way 2");
    // https_request_data_Fun (2,body);
    // ESP_LOGI(TAG, "way 3");
    // https_request_data_Fun (3,body);

    JSON_DataToAtmajson (0,"9966","E2082734893739367282","SN-123123","urn:atma.io:loc:rovinj:rovinj:9018",2,4,http_array);
    https_cache_request_data_Fun (1,http_array);
    while (1)
    {
        if (https_enable)
        {
            https_cache_request_State_machine (NULL);
        }
        http_cache_port_State_machine (NULL);   // 暂时把http放在https里面 

        times ++;
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
