#ifndef _DATA__H_
#define _DATA__H_

/*
        【API】      ————C语言环境即可运行
        本文件用于各种数据处理，它一定会很庞大的             ————2022.6.20
    Over_Time       数据超时函数

    API不兼容                       ————2022.6.20
    
*/

#include "Caven_Type.h"

typedef struct
{
    char *Source;
    char Sign[8];           //取数符
    float NUM[8];           //取数结果
    int flag;
}Sign_atof_Type;


int Data_Sign_atof_Handle (Sign_atof_Type *Item);
int Caven_find_string_array (char const *str,char const * target,int length);
int Caven_Data_Median_filtering_Handle (float data,float *array,float *reverse,char *run,char array_num);
int Caven_math_approximate (int num,int num_step,int num_min,int num_max);
int Caven_math_approximate_float(float num);

#endif

