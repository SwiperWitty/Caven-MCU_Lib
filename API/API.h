#ifndef _API__H_
#define _API__H_

#include "keyboard.h"

/* 数据处理 */
#include "DATA_Handle.h"
#include "Check_CRC.h"
#include "Caven_info_frame.h"

/* 时间处理 */
#include "Time_Handle.h"

/* 事件处理 */
#include "Caven_event_frame.h"

/* 外部组件 */
#include "Vofa_info.h"

#include "Caven_Type.h"

/********************************/

/*
    API会与底层隔离，c 环境即可运行
    结构体数据 (_xxx_xx)
    结构体函数 (xxx_xx_)
*/
/*
    SDK->Items->GPIO(Exist_GPIO)->BASE->
                                         \\
                                          -->[XXX]->MODE
                                         //
                    C(Lib)->Caven->API->
*/

struct API_Data_
{
    unsigned short (*Get_CRC16)(unsigned char *ptr, int len);
    
    struct Data_Handle_ Data;
    
};

extern struct API_Data_ API;

//这个文件是不需要【初始化结构体】的，只有初始化索引

void API_Index(void);

#endif
