#ifndef _DATA__H_
#define _DATA__H_

/*
        【API】      ————C语言环境即可运行
        本文件用于各种数据处理，它一定会很庞大的             ————2022.6.20
    Over_Time       数据超时函数

    API不兼容                       ————2022.6.20
    #include "DATA_Handle.h"
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

int data_gain_str_by_sign(char *file,int file_len,char *pData,char *sign_str,char sign);

int Caven_String_To_Hex (char *str);
int Caven_Hex_To_String (uint8_t *array,int len,char *ret_str);
int Caven_Str_To_ip (char *str,uint8_t *ip,int IpSize);
int Caven_Data_Median_filtering_Handle (float data,float *array,float *reverse,char *run,char array_num);

int Caven_math_approximate (int num,int num_step,int num_min,int num_max);
int Caven_math_approximate_float(float num);

#endif

