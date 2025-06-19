#ifndef _IC__HC138_H_
#define _IC__HC138_H_

#include "stdint.h"
#include "Items.h"
#if Exist_HC138
	#include "Base.h"
#endif
#include "Caven_Type.h"

/*
    38-译码器（8选1）
*/
#ifdef Exist_HC595

#define HC138_GPIO      3

#define HC138_D1_IO     1
#define HC138_D2_IO     2
#define HC138_D3_IO     3

typedef struct
{
    void (*Set_DATA) (int Data);
}MODE_HC138_Way;
#endif

void MODE_HC138_Init (int Set);
void HC138_Set_Data_Fun (int Data);

#endif
