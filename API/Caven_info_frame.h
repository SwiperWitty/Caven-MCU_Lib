#ifndef _CAVEN_INFO_FRAME__H_
#define _CAVEN_INFO_FRAME__H_

#ifdef CAVEN    /* 预编译 */
#include "Caven_Type.h"
#else
#include "stdint.h"
#endif

/*
    Caven_info_frame.h
    描述Caven的通信协议规范
    可能需要依赖外部的crc校验函数
*/


typedef struct
{
    unsigned short Head;    //2byte

    unsigned char Versions; //1byte 3
    unsigned char Type;     //1byte 4
    unsigned char Addr;     //1byte 5
    unsigned char Cmd;      //1byte 6
    unsigned char Cmd_sub;  //1byte 7

    unsigned short Size;    //2byte 9
    unsigned char *p_Data;  // 

    unsigned char Result;   //1byte 

    unsigned short End_crc; //2byte 
}Caven_info_packet_Type; 


int Caven_info_frame_Fun (Caven_info_packet_Type const standard,Caven_info_packet_Type *target,uint16_t data , char channel);

#endif
