#ifndef _HC138_H_
#define _HC138_H_

#include "Base.h"
#include "Caven_Type.h"

/*
    38-译码器（8选1）
    管脚定义在 Base_Exist_GPIO 文件
*/

typedef struct
{
    void (*Set_DATA) (int Data);
}MODE_HC138_Way;

void MODE_HC138_Init (int SET);
void HC138_Set_Data_Fun (int Data);

#endif
