#ifndef _CAVEN_INFO_FRAME__H_
#define _CAVEN_INFO_FRAME__H_

#ifdef CAVEN    /* 预编译 */
#include "Caven_Type.h"
#else
#include "stdint.h"
#include "string.h"
#include "stddef.h"
#include "stdio.h"
#endif

/*
    Caven_info_frame.h
    描述info的通信协议规范
    可能需要依赖外部的crc校验函数[Encrypt_XMODEM_CRC16_Fun]

    tips:
    1).中断里面调用的函数，其他地方不要调用，特别是带有memcpy的函数，你会发现同一个功能，但是名称多一个fast。
    2).中断里面不要消耗算力的函数，即使它看起来很简单。例如CRC。
    3).(clean) while -> send -> [do] ->while -> ... YYDS,其实就是将消耗算力的东西（CRC）放在要while之前，这样可以do and do
*/

/*
    此结构体包含指针变量，所以此结构体需要初始化其中的指针。
*/
typedef struct
{
    uint16_t Head;    //2byte

    uint8_t Versions; //1byte 3
    uint8_t Type;     //1byte 4
    uint8_t Addr;     //1byte 5
    uint8_t Cmd;      //1byte 6
    uint8_t Cmd_sub;  //1byte 7

    uint16_t dSize;   //2byte 9
    uint8_t *p_AllData;   // min 12byte buff
    uint8_t *p_Data;      //
    uint16_t get_crc; //2byte
    uint16_t end_crc; //2byte

    uint8_t Result;   //1byte
    int Run_status;
    uint16_t Get_num;

    uint8_t Comm_way;
}Caven_info_packet_Type;

/*
 * CAVEN_CMD
 */
enum CAVEN_CMD1
{
    m_CAVEN_CMD1_TEST_Order = 0x00,
    m_CAVEN_CMD1_Version_Order,
    m_CAVEN_CMD1_Serial_Order,
	m_CAVEN_CMD1_Model_Order,
    m_CAVEN_CMD1_Bdtime_Order,
    m_CAVEN_CMD1_UTCtime_Order,
    m_CAVEN_CMD1_Addr_Order,
    m_CAVEN_CMD1_RS232Cfg_Order,
    m_CAVEN_CMD1_RS485Cfg_Order,
    m_CAVEN_CMD1_TCPHBT_Order,
    m_CAVEN_CMD1_IPv4Cfg_Order,
    m_CAVEN_CMD1_TCPServer_Order,
    m_CAVEN_CMD1_TCPClient_Order,
    m_CAVEN_CMD1_HTTPHBT_Order,
    m_CAVEN_CMD1_HTTPCfg_Order,
    m_CAVEN_CMD1_MQTTCfg_Order,
    m_CAVEN_CMD1_GetMAC_Order,
    m_CAVEN_CMD1_CANCfg_Order,
    m_CAVEN_CMD1_Reset_Order,
    m_CAVEN_CMD1_GetWork_Order,
    m_CAVEN_CMD1_GetNet_Order,
    m_CAVEN_CMD1_BLECfg_Order,
    m_CAVEN_CMD1_NetCardCfg_Order,
    m_CAVEN_CMD1_NetCardIP_Order,
    m_CAVEN_CMD1_UDPCfg_Order,
    m_CAVEN_CMD1_UDPMulticast_Order,
    m_CAVEN_CMD1_UDPUpdata_Order,
	m_CAVEN_CMD1_TCPUpHtdata_Order,
	m_CAVEN_CMD1_Worktime_Order,
};

enum CAVEN_CMD2
{
    m_CAVEN_CMD2_TEST_Order = 0x00,
    m_CAVEN_CMD2_MACCfg_Order,
    m_CAVEN_CMD2_BTLD_Order,
    m_CAVEN_CMD2_OTA_Order,
    m_CAVEN_CMD2_Reset_Order,
    m_CAVEN_CMD2_DEBUG_Order = 0x05,
    m_CAVEN_CMD2_Restore_Order,
    m_CAVEN_CMD2_Serial_Order,
};

enum CAVEN_CMD3
{
    m_CAVEN_CMD3_TEST_Order = 0x00,
    m_CAVEN_CMD3_BEEP_Order,
    m_CAVEN_CMD3_GPOCfg_Order,
    m_CAVEN_CMD3_GPIGet_Order,
    m_CAVEN_CMD3_GPIUpdata_Order,
    m_CAVEN_CMD3_GPIToCMD_Order = 0x05,
    m_CAVEN_CMD3_Channel_Retell_Order,
    m_CAVEN_CMD3_Channel_Updata_Order,

    m_CAVEN_CMD3_zhiwen_Order = 0x1B,
};

/*
 * CAVEN_Status and event
 */
enum CAVEN_STATUS
{
    m_CAVEN_IDLE_Stat = 0,
    m_CAVEN_CHAOS_Stat,
    m_CAVEN_COMM_Stat,
    m_CAVEN_BUSY_Stat,
    m_CAVEN_CHOKE_Stat,
    m_CAVEN_DOWN_Stat,
};

typedef enum
{
    m_Result_Back_Succ = 0,
    m_Result_Fail_Vers,
    m_Result_Fail_Type,
    m_Result_Fail_Addr,
    m_Result_Fail_CMD,
    m_Result_Fail_CMDS,
    m_Result_Fail_Leng,
    m_Result_Fail_CRC,
    m_Result_Fail_Spoil,    // 消息损坏
    m_Result_Fail_ERROR,    // 执行失败了
    m_Result_Back_Other = 0x0A,
    m_Result_Back_Empty = 0x0F,	// 静默执行
    m_Result_SUCC = 0x50,   // 完整一帧
}Caven_info_Result_mType;

#ifndef BUFF_MAX
    #define BUFF_MAX  500
#endif // !BUFF_MAX

int Caven_info_packet_clean_Fun(Caven_info_packet_Type *target);
int Caven_info_packet_fast_clean_Fun(Caven_info_packet_Type *target);

int Caven_info_packet_index_Fun(Caven_info_packet_Type *target, uint8_t *data);
int Caven_packet_data_copy_Fun(Caven_info_packet_Type *source,Caven_info_packet_Type target);

int Caven_Circular_queue_input (Caven_info_packet_Type data,Caven_info_packet_Type *Buff_data,int Buff_Num);
int Caven_Circular_queue_output(Caven_info_packet_Type *data,Caven_info_packet_Type *Buff_data,int Buff_Num);

int Caven_info_Make_packet_Fun(Caven_info_packet_Type const standard, Caven_info_packet_Type *target, uint8_t data);
int Caven_info_Split_packet_Fun(Caven_info_packet_Type const source, uint8_t *data);
int Caven_info_return_Fun (uint8_t Ver,uint8_t Type,uint8_t Addr,uint8_t Cmd,uint8_t Cmd_sub,uint16_t len,uint8_t *data,uint8_t Result,uint8_t *array);


#endif
