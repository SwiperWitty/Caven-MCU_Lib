#ifndef _IC__HC595_H_
#define _IC__HC595_H_

#include "stdint.h"
#include "Items.h"
#if Exist_HC595
	#include "Base.h"
#endif
#include "Caven_Type.h"

/*
	本文档库是 Otavia用于 控制 Zeno的 74CH595驱动程序、其兼容了级联输出（多个缓冲器）
*/
#ifdef Exist_HC595

#define HC595_GPIO      3

#define HC595_SCK_IO    1
#define HC595_Data_IO   2
#define HC595_RCK_IO    3

typedef struct
{
    void (*Set_DATA) (const uint8_t *Dat,char num);
}MODE_HC595_Way;
#endif

void MODE_HC595_Init (int set);
void HC595_Set_DATA_Fun (const uint8_t *Dat,char num);


#endif
