#ifndef _IC_AS608__H_
#define _IC_AS608__H_

#include "stdint.h"
#include "string.h"
#include "Items.h"

// 指令定义
#define AS608_CMD_HEADER            0xEF01

// 指令码
#define AS608_CMD_GENIMG           0x01  // 生成图像
#define AS608_CMD_IMG2CHAR         0x02  // 生成特征
#define AS608_CMD_MATCH            0x03  // 精确比对
#define AS608_CMD_SEARCH           0x04  // 搜索
#define AS608_CMD_REGMODEL         0x05  // 合并特征（生成模板）
#define AS608_CMD_STORE            0x06  // 存储模板
#define AS608_CMD_LOAD             0x07  // 读出模板
#define AS608_CMD_UPCHAR           0x08  // 上传特征
#define AS608_CMD_DOWNCHAR         0x09  // 下载特征
#define AS608_CMD_UPIMG            0x0A  // 上传图像
#define AS608_CMD_DOWNIMG          0x0B  // 下载图像
#define AS608_CMD_DELETE           0x0C  // 删除模板
#define AS608_CMD_EMPTY            0x0D  // 清空指纹库
#define AS608_CMD_READ_SYS_PARA    0x0F  // 读系统参数
#define AS608_CMD_SET_PWD          0x12  // 设置密码
#define AS608_CMD_VERIFY_PWD       0x13  // 验证密码
#define AS608_CMD_GET_RANDOM_CODE  0x14  // 产生随机数
#define AS608_CMD_SET_CHIP_ADDR    0x15  // 设置模块地址
#define AS608_CMD_READ_INFO_PAGE   0x16  // 读信息库
#define AS608_CMD_TEMPLATE_NUM     0x1D  // 读指纹库模板个数
#define AS608_CMD_GET_ALGORITHM    0x28  // 读算法库版本
#define AS608_CMD_LED_CONTROL      0x35  // LED控制

// 确认码
#define AS608_ACK_SUCCESS          0x00  // 指令执行成功
#define AS608_ACK_FAIL             0x01  // 指令执行失败
#define AS608_ACK_FULL             0x04  // 指纹库满
#define AS608_ACK_NO_USER          0x05  // 无此用户
#define AS608_ACK_USER_OCCUPIED    0x06  // 用户已存在
#define AS608_ACK_TIMEOUT          0x08  // 采集超时
#define AS608_ACK_GEN_IMG_FAIL     0x09  // 生成图像失败

// 参数
#define AS608_MAX_TEMPLATES        300   // 最大指纹模板数
#define AS608_PACKET_SIZE          32    // 数据包大小

// 结构体定义
typedef struct {
    uint16_t header;
    uint32_t address;
    uint8_t  type;
    uint16_t length;
    uint8_t  data[AS608_PACKET_SIZE];
    uint16_t checksum;
    uint16_t get_crc;
    uint16_t get_num;
    uint16_t dSize;
    int Run_status;
} AS608_Packet;

typedef struct {
    uint16_t status;
    uint16_t library_size;
    uint8_t  security_level;
    uint16_t packet_size;
    uint32_t baudrate;
} AS608_System_Params;

int AS608_info_Make_packet_Fun(AS608_Packet *target, uint8_t data);
int AS608_info_Split_packet_Fun(AS608_Packet const source, uint8_t *data);

int AS608_info_packet_fast_clean_Fun(AS608_Packet *target);

#endif
