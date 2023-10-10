#ifndef _Check_CRC16_H_
#define _Check_CRC16_H_

#define CRC_16_CCITT            0x1021

#include "Caven_Type.h"
/*
        ��API��      ��������C���Ի�����������
    Over_Time       ���ݳ�ʱ����


*/

unsigned short CRC16_CCITT_CalculateBuf(unsigned char *ptr, int len);

#endif
