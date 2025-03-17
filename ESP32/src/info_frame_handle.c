#include "info_frame_handle.h"

/*
    ESP_LOGE - 错误（最低）0
    ESP_LOGW - 警告
    ESP_LOGI - 信息
    ESP_LOGD - 调试
    ESP_LOGV - 详细（最高）
*/

int receive_info_frame_handle (const void * data, int size, char channel)
{
    int retval = 0;
    uint8_t data_buff[500];
    memcpy(data_buff,data,size);
    for (int i = 0; i < size; i++)
    {
#if (PROTOCOL_FRAME == m_Protocol_CV)
    // caven

#elif (PROTOCOL_FRAME == m_Protocol_GX)
    // gx

#endif // 
    }
    
    return retval;
}

int send_info_frame_handle (const void * data, int size, char channel)
{
    int retval = 0;

#if (PROTOCOL_FRAME == m_Protocol_CV)
    // caven

#elif (PROTOCOL_FRAME == m_Protocol_GX)
    // gx

#endif // DEBUG

    
    return retval;
}
