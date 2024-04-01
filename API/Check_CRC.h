#ifndef _Check_CRC16_H_
#define _Check_CRC16_H_


#ifdef CAVEN
#include "Caven_Type.h"
#else
#include "stdint.h"
#endif

/*
    ModBus_CRC16用的
*/
#define CRC_16_CCITT            0x1021

int ModBus_CRC16(unsigned char *data, int len);
int CRC16_CCITT_fast_Fun(unsigned char *data, int len);

#endif
