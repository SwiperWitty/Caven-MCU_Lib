#include "MODE_uart.h"
#include "string.h"
/*
    ESP_LOGE - 错误（最低）0
    ESP_LOGW - 警告
    ESP_LOGI - 信息
    ESP_LOGD - 调试
    ESP_LOGV - 详细（最高）
*/

static const char *TAG = "MODE_UART";
QueueHandle_t uart1_event_queue;
QueueHandle_t uart2_event_queue;

static int custom_uart1_init_flag = 0;
static int custom_uart2_init_flag = 0;

int custom_uart1_init(int baud_rate, int set)
{
    int retval = 0;
    if (set)
    {
        uart_config_t uart_config = {
            .baud_rate = baud_rate,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
        uart_param_config(UART_NUM_1, &uart_config);
        uart_set_pin(UART_NUM_1, ECHO_TXD1, ECHO_RXD1, ECHO_TEST_RTS, ECHO_TEST_CTS);
        // 串口1的接收缓冲区2048，发送缓冲区2048，定义uart1_event_queue 事件消息队列大小为10
        uart_driver_install(UART_NUM_1, 2048, 2048, 10, &uart1_event_queue, 0);
        custom_uart1_init_flag = 1;
    }
    else
    {
        custom_uart1_init_flag = 0;
    }
    ESP_LOGI(TAG, "1 init %d ", retval);
    return retval;
}

int custom_uart2_init(int baud_rate, int set)
{
    int retval = 0;
    if (set)
    {
        uart_config_t uart_config = {
            .baud_rate = baud_rate,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
        uart_param_config(UART_NUM_2, &uart_config);
        uart_set_pin(UART_NUM_2, ECHO_TXD2, ECHO_RXD2, ECHO_TEST_RTS, ECHO_TEST_CTS);
        // 串口2的接收缓冲区2048，发送缓冲区2048，定义uart2_event_queue 事件消息队列大小为10
        uart_driver_install(UART_NUM_2, 2048, 2048, 10, &uart2_event_queue, 0);
        custom_uart2_init_flag = 1;
    }
    else
    {
        custom_uart2_init_flag = 0;
    }
    ESP_LOGI(TAG, "2 init %d ", retval);
    return retval;
}

/*
    使用前确保初始化
*/
int custom_uart1_send_data(uint8_t *data, int size)
{
    int retval = 0;
    int len = size;
    if (data == NULL || size == 0)
    {
        retval = 1;
        return retval;
    }
    
    if (custom_uart1_init_flag > 0)
    {
        uart_write_bytes(UART_NUM_1, data, len);
    }
    return retval;
}

/*
    使用前确保初始化
*/
int custom_uart2_send_data(uint8_t *data, int size)
{
    int retval = 0;
    int len = size;
    if (data == NULL || size == 0)
    {
        retval = 1;
        return retval;
    }
    
    if (custom_uart2_init_flag > 0)
    {
        uart_write_bytes(UART_NUM_2, data, len);
    }
    return retval;
}

/*
    接收回调函数执行指针
*/
static D_Callback_pFun custom_uart1_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void custom_uart1_receive_State_Machine_Bind (D_Callback_pFun Callback_pFun)
{
    custom_uart1_Callback_Fun = Callback_pFun;
}

/*
    接收回调函数执行指针
*/
static D_Callback_pFun custom_uart2_Callback_Fun = NULL;
/*
    接收回调函数绑定
*/
void custom_uart2_receive_State_Machine_Bind (D_Callback_pFun Callback_pFun)
{
    custom_uart2_Callback_Fun = Callback_pFun;
}

// 
int uart1_task_num = 0;
void uart1_task(void *pvParam)
{
    EventBits_t r_event;
    uart_event_t event;
    TickType_t absolute_Time = 1;
    int counter = 0;
    uint8_t data[512];
    if (custom_uart1_init_flag == 0)
    {
        custom_uart1_init(115200, 1);
    }
    memset(data, 0, sizeof(data));
    while (1)
    {
        // Waiting for UART event.
        if (xQueueReceive(uart1_event_queue, (void *)&event, 10))
        {
            switch (event.type)
            {
            case UART_DATA:
            {
            }
            case UART_BUFFER_FULL:
            {
                counter = uart_read_bytes(UART_NUM_1, data, 512, 0);
                if (counter > 0)
                {
                    uart1_task_num += counter;
                    if (custom_uart1_Callback_Fun != NULL)
                    {
                        for (int i = 0; i < counter; i++)
                        {
                            custom_uart1_Callback_Fun(data + i);
                        }
                    }
                    
                    // ESP_LOGI(TAG, "uart1 get %d ", counter);
                }
            }
            break;
            default:
                break;
            }
        }
        else
        {
        }
        vTaskDelay(pdMS_TO_TICKS(absolute_Time));
    }
    vTaskDelete(NULL); /*  基本不用退出 */
}

//
int uart2_task_num = 0;
void uart2_task(void *pvParam)
{
    EventBits_t r_event;
    uart_event_t event;
    TickType_t absolute_Time = 1;
    int counter = 0;
    uint8_t data[512];
    if (custom_uart2_init_flag == 0)
    {
        custom_uart2_init(115200, 1);
    }
    memset(data, 0, sizeof(data));
    while (1)
    {
        // Waiting for UART event.
        if (xQueueReceive(uart2_event_queue, (void *)&event, 10))
        {
            switch (event.type)
            {
            case UART_DATA:
            {
            }
            case UART_BUFFER_FULL:
            {
                counter = uart_read_bytes(UART_NUM_2, data, 512, 0);
                if (counter > 0)
                {
                    uart2_task_num += counter;
                    if (custom_uart2_Callback_Fun != NULL)
                    {
                        for (int i = 0; i < counter; i++)
                        {
                            custom_uart2_Callback_Fun(data + i);
                        }
                    }
                    
                    // ESP_LOGI(TAG, "uart2 get %d ", counter);
                }
            }
            break;
            default:
                break;
            }
        }
        else
        {
        }
        vTaskDelay(pdMS_TO_TICKS(absolute_Time));
    }
    vTaskDelete(NULL); /*  基本不用退出 */
}

void custom_uart_task_Fun(void)
{

    if (custom_uart1_init_flag)
    {
        xTaskCreate(uart1_task, "task-[uart1]", 4096 * 1, NULL, UART1_TASK_PRIORITY, NULL);
    }
    if (custom_uart2_init_flag)
    {
        xTaskCreate(uart2_task, "task-[uart2]", 4096 * 1, NULL, UART2_TASK_PRIORITY, NULL);
    }
}
