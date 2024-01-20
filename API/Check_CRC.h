#ifndef _Check_CRC16_H_
#define _Check_CRC16_H_

#define CRC_16_CCITT            0x1021

#ifdef CAVEN
#include "Caven_Type.h"
#else
#include "stdint.h"
#endif
/*


*/
int ModBusCRC16(unsigned char *data, int len);
unsigned short CRC16_CCITT_CalculateBuf(unsigned char *ptr, int len);

#endif
