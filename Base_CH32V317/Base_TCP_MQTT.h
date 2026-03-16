#ifndef BASE_TCP_MQTT_H
#define BASE_TCP_MQTT_H

#if DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif

#include "Caven_Type.h"

/************************ 函数声明 ************************/

/**
 * @brief 配置MQTT客户端
 * @param url: MQTT服务器URL，格式支持 "tcp://ip:port/topic" 或 "ip:port/topic"
 * url<tcp://dianjixz.online:1883>U<nihao>P<nihao3.14>Ptopic<send>Stopic<reace>Id<caven_test>
 * @param enable: 1-启用MQTT客户端，0-禁用
 * @return 0-成功，其他-失败
 */
int Base_TCP_MQTT_Config(char *config, int enable);

/**
 * @brief 放入待发送的MQTT数据到缓存
 * @param data: 待发送的数据指针
 * @param len: 待发送数据长度
 * @return 0-成功，其他-失败
 */
int Base_TCP_MQTT_cache_Send_Fun(char *data, int len);

/**
 * @brief 读取MQTT接收到的应用消息
 * @param data: 接收数据的缓冲区指针
 * @param len_max: 缓冲区最大长度
 * @return 实际读取到的字节数，0-无数据
 */
int Base_TCP_MQTT_cache_Read_Fun(char *data, int len_max);

/**
 * @brief MQTT主任务处理函数（需周期性调用）
 * @param noway: 占位参数（兼容ETH回调接口）
 * @param nobady: 占位参数（兼容ETH回调接口）
 * @return 无
 */
void Base_TCP_MQTT_Task(u8 noway, u8 nobady);

#endif // BASE_TCP_MQTT_H
