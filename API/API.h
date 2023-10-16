#ifndef _API__H_
#define _API__H_

#include "Check_CRC.h"
#include "Time_Handle.h"
#include "DATA_Handle.h"
#include "keyboard.h"


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
    
    struct Time_Handle_ Time;
    struct Data_Handle_ Data;
    
};

extern struct API_Data_ API;

//这个文件是不需要【初始化结构体】的，只有初始化索引

void API_Index(void);

#endif
