#ifndef _CAVEN_AT_INFO_FRAME__H_
#define _CAVEN_AT_INFO_FRAME__H_

#include "stdint.h"
#include "string.h"

/*
    此结构体包含指针变量，所以此结构体需要初始化其中的指针。
*/
typedef struct
{
    uint16_t Head;          //2byte
    uint16_t End;

    uint16_t dSize;         //2byte
    uint8_t *p_Data;

    int Result;            //1byte 
    int Run_status;
    uint16_t Get_num;

    uint8_t Comm_way;
}Caven_at_info_packet_Type;

int caven_at_info_Make_packet_Fun(Caven_at_info_packet_Type const standard, Caven_at_info_packet_Type *target, uint8_t data);
int caven_at_info_Split_packet_Fun(Caven_at_info_packet_Type const target, uint8_t *data);
int caven_at_info_packet_index_Fun(Caven_at_info_packet_Type *target, uint8_t *data);
int caven_at_info_packet_clean_Fun(Caven_at_info_packet_Type *target);
int caven_at_info_packet_fast_clean_Fun(Caven_at_info_packet_Type *target);

#endif
