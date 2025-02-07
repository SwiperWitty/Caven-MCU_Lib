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
enum CAVEN_CMD
{
    m_CAVEN_TEST_Order = 0x00,
    m_CAVEN_SYS_Order,
    m_CAVEN_BOOT_Order,
    m_CAVEN_FEATURE_Order,
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
    m_Result_Back_Vers,
    m_Result_Fail_Type,
    m_Result_Fail_Addr,
    m_Result_Back_CMD,
    m_Result_Back_CMDS,
    m_Result_Back_Leng,
    m_Result_Back_CRC,
    m_Result_Fail_Spoil,
    m_Result_Back_Other,    // 执行失败了
    m_Result_Back_Empty,    // 我可以发出去，但是你不可以返回
    m_Result_Fail_Empty,    // 这条消息不返回
}Caven_info_Result_mType;
#define RESULT_DEFAULT  m_Result_Fail_Empty

#ifndef BUFF_MAX
    #define BUFF_MAX  500
#endif // !BUFF_MAX

int Caven_info_packet_clean_Fun(Caven_info_packet_Type *target);
int Caven_info_packet_fast_clean_Fun(Caven_info_packet_Type *target);

int Caven_info_packet_index_Fun(Caven_info_packet_Type *target, uint8_t *data);
int Caven_packet_data_copy_Fun(Caven_info_packet_Type *source,Caven_info_packet_Type *target);

int Caven_Circular_queue_input (Caven_info_packet_Type *data,Caven_info_packet_Type *Buff_data,int Buff_Num);
int Caven_Circular_queue_output(Caven_info_packet_Type *data,Caven_info_packet_Type *Buff_data,int Buff_Num);

int Caven_info_Make_packet_Fun(Caven_info_packet_Type const standard, Caven_info_packet_Type *target, uint8_t data);
int Caven_info_Split_packet_Fun(Caven_info_packet_Type const source, uint8_t *data);



#endif
