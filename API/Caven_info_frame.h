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
    描述Caven的通信协议规范
    可能需要依赖外部的crc校验函数
*/

/*
    此结构体包含指针变量，所以此结构体需要初始化其中的指针。
*/
typedef struct
{
    unsigned short Head;    //2byte

    unsigned char Versions; //1byte 3
    unsigned char Type;     //1byte 4
    unsigned char Addr;     //1byte 5
    unsigned char Cmd;      //1byte 6
    unsigned char Cmd_sub;  //1byte 7

    unsigned short dSize;   //2byte 9
    unsigned char *p_Data;  // min 12byte buff

    unsigned char Result;   //1byte 

    unsigned short End_crc; //2byte 

    int Run_status;
    unsigned short Get_num;

    unsigned char Comm_way;
}Caven_info_packet_Type; 

#define RESULT_DEFAULT  10

int Caven_info_packet_clean_Fun(Caven_info_packet_Type *target);
int Caven_info_packet_index_Fun(Caven_info_packet_Type *target, unsigned char *data);

int Caven_info_Make_packet_Fun(Caven_info_packet_Type const standard, Caven_info_packet_Type *target, unsigned char data);
int Caven_info_Split_packet_Fun(Caven_info_packet_Type const source, unsigned char *data);



#endif
