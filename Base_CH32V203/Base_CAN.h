#ifndef __BASE_CAN__H_
#define __BASE_CAN__H_

#ifdef DEFAULT
#include "Items.h"              // 默认功能
#else
#include "User_items.h"         // 自行设置功能，一般出现在本地文件的User中
#endif
#include "Caven_Type.h"

/*  Init-Function    */
int Base_CAN_Init(uint8_t Width,int Set);
int Base_CAN_Data (const uint8_t *data,int size);
int Base_CAN_Bind (D_pFun Callback_pFun);

// end 
#endif
