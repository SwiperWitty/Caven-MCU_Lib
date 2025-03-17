#include "User_MQTT.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "system_parameter.h"
#include "reader_typedef.h"


static const char *TAG = "MQTT_EXAMPLE";


static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}
void Z_Mqtt_Init(void);
 
bool Z_mqtt_connect_flag=false;             //记录是否连接上MQTT服务器的一个标志,如果连接上了才可以发布信息
esp_mqtt_client_handle_t emcht;             //MQTT客户端句柄
 
extern int frame_recv_data_analyse(uint8_t recv_channel, uint8_t *recv_data, uint16_t recv_data_length);

void TCP_MQTT_send (uint8_t *data,int len)
{
    if(Z_mqtt_connect_flag == true && data != NULL) 
    {
        esp_mqtt_client_publish(emcht,"/topic/qos1",(char *)data,len,1,0);
    }
}
void TCP_MQTT_receive (char *data,int len)
{
    uint8_t temp_data;
    if(Z_mqtt_connect_flag == true && data != NULL) 
    {
        esp_mqtt_client_publish(emcht,"/topic/qos1",(char *)data,len,1,0);
        for (int i = 0; i < len; i++)
        {
            temp_data = data[i];
            frame_recv_data_analyse(TCP_LINK_MQTT, &temp_data, 1);
        }
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/topic/qos1","test mqtt",0, 1, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        // ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        // msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        // ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        Z_mqtt_connect_flag = true;
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        Z_mqtt_connect_flag = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        TCP_MQTT_receive (event->data,event->data_len);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
 
void Z_Mqtt_Init(void){
    if (system_para.MQTT_able)
    {
        ESP_LOGI(TAG, "Z_Mqtt_Init ");
        esp_mqtt_client_config_t emcct = {
            .uri=system_para.MQTT_URL,   //MQTT服务器的uri
            // .port=system_para.MQTT_port, //MQTT服务器的端口
        };
        emcht = esp_mqtt_client_init(&emcct);           //初始化MQTT客户端获取句柄
        if(!emcht)  printf("mqtt init error!\r\n");
        
        //注册MQTT事件处理函数
        if(esp_mqtt_client_register_event(emcht,ESP_EVENT_ANY_ID,mqtt_event_handler,NULL)!=ESP_OK)  printf("mqtt register error!\r\n");
    
        //开启MQTT客户端
        if(esp_mqtt_client_start(emcht) != ESP_OK)  printf("mqtt start errpr!\r\n");

    }
}

/*
    网络的应用层任务必须先确保底层网络是启动的，否则不应该启动这个任务
*/
void TCP_MQTT_task(void)
{
    int temp_num = 0;
    int link_wan = 0;
    while(1)
    {
        //每隔5S发布一次测试消息
        temp_num = system_para.rj45_connect_state + system_para.wifi_connect_state;
        if (temp_num && link_wan == 0 && system_para.MQTT_able)
        {
            link_wan = 1;
            Z_Mqtt_Init();
            vTaskDelay(200);
        }
        else if (temp_num == 0 || system_para.MQTT_able == 0)
        {
            if (emcht != NULL)
            {
                ESP_LOGI(TAG, "Z_Mqtt exit ");
                esp_mqtt_client_stop(emcht);
                esp_mqtt_client_disconnect(emcht);
                emcht = NULL;
            }
            link_wan = 0;
        }

        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}
