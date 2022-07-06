#ifndef _DATA__H_
#define _DATA__H_

/*
    本文件用于各种数据处理，它一定会很庞大的             ————2022.6.20
    Over_Time       数据超时函数

    API不兼容                       ————2022.6.20
    
*/

#include "Caven.h"

struct Data_Handle_
{
    int (*Find_Data)(const U8 *source, char Target, int Length);
    
};

int Find_Data(const U8 *source, char Target, int Length);


#endif

