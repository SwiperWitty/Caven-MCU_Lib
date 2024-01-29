#ifndef _HC595_H_
#define _HC595_H_

#include "Base.h"
#include "Caven_Type.h"

/*
	本文档库是 Otavia用于 控制 Zeno的 74CH595驱动程序、其兼容了级联输出（多个缓冲器）
    管脚定义在 Base_Exist_GPIO 文件
*/

typedef struct
{
    void (*Set_DATA) (const unsigned char *Dat,char num);
}MODE_HC595_Way;

void MODE_HC595_Init (int SET);
void HC595_Set_DATA_Fun (const unsigned char *Dat,char num);


#endif
